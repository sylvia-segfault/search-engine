/*
 * Copyright ©2020 Hal Perkins.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Spring Quarter 2020 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <string>

#include "./HttpConnection.h"

#include "gtest/gtest.h"
#include "./HttpRequest.h"
#include "./HttpResponse.h"
#include "./HttpUtils.h"
#include "./test_suite.h"

using std::string;

namespace hw4 {

TEST(Test_HttpConnection, TestHttpConnectionBasic) {
  // Create a socketpair; we'll hand one end of the socket to the
  // HttpConnection object, and use the other end of the socket
  // ourselves for testing.
  int spair[2] = {-1, -1};
  ASSERT_EQ(0, socketpair(AF_UNIX, SOCK_STREAM, 0, spair));

  // Create the HttpConnection object we'll test.
  HttpConnection hc(spair[0]);

  // Write three requests on the socket.
  string req1 = "GET /foo HTTP/1.1\r\n";
  req1 += "Host: somehost.foo.bar\r\n";
  req1 += "Connection: close\r\n";
  req1 += "\r\n";
  // req2: check header order doesn't matter
  string req2 = "GET /bar HTTP/1.1\r\n";
  req2 += "Connection: close\r\n";
  req2 += "Host: somehost.foo.bar\r\n";
  req2 +=  "\r\n";
  // req3: check different values
  string req3 = "GET /baz HTTP/1.1\r\n";
  req3 += "connection: keep-alive\r\n";
  req3 += "host: somehost.foo.bar\r\n";
  req3 += "OTHER: some_value\r\n";
  req3 +=  "\r\n";
  ASSERT_EQ(static_cast<int>(req1.size()),
            WrappedWrite(spair[1],
                         (unsigned char *) req1.c_str(),
                         static_cast<int>(req1.size())));
  ASSERT_EQ(static_cast<int>(req2.size()),
            WrappedWrite(spair[1],
                         (unsigned char *) req2.c_str(),
                         static_cast<int>(req2.size())));
  ASSERT_EQ(static_cast<int>(req3.size()),
            WrappedWrite(spair[1],
                         (unsigned char *) req3.c_str(),
                         static_cast<int>(req3.size())));

  // Do the GetNextRequests.
  HttpRequest htreq1, htreq2, htreq3;
  ASSERT_TRUE(hc.GetNextRequest(&htreq1));
  ASSERT_TRUE(hc.GetNextRequest(&htreq2));
  ASSERT_TRUE(hc.GetNextRequest(&htreq3));
  HW4Environment::AddPoints(30);

  // Make sure the request parsing worked.
  ASSERT_EQ("/foo", htreq1.uri());
  ASSERT_EQ("somehost.foo.bar", htreq1.GetHeaderValue("host"));
  ASSERT_EQ("close", htreq1.GetHeaderValue("connection"));
  ASSERT_EQ(2, htreq1.GetHeaderCount());

  ASSERT_EQ("/bar", htreq2.uri());
  ASSERT_EQ("somehost.foo.bar", htreq2.GetHeaderValue("host"));
  ASSERT_EQ("close", htreq2.GetHeaderValue("connection"));
  ASSERT_EQ(2, htreq2.GetHeaderCount());

  ASSERT_EQ("/baz", htreq3.uri());
  ASSERT_EQ("somehost.foo.bar", htreq3.GetHeaderValue("host"));
  ASSERT_EQ("keep-alive", htreq3.GetHeaderValue("connection"));
  ASSERT_EQ("some_value", htreq3.GetHeaderValue("other"));
  ASSERT_EQ(3, htreq3.GetHeaderCount());
  HW4Environment::AddPoints(30);

  // Prepare the responses.
  HttpResponse rep1;
  rep1.set_protocol("HTTP/1.1");
  rep1.set_response_code(200);
  rep1.set_message("OK");
  rep1.AppendToBody("This is the body of the response.");
  string expectedrep1 = "HTTP/1.1 200 OK\r\n";
  expectedrep1 += "Content-length: 33\r\n\r\n";
  expectedrep1 += "This is the body of the response.";

  HttpResponse rep2;
  rep2.set_protocol("HTTP/1.1");
  rep2.set_response_code(200);
  rep2.set_message("OK");
  rep2.set_content_type("text/html");
  rep2.AppendToBody("This is the second response.");
  string expectedrep2 = "HTTP/1.1 200 OK\r\n";
  expectedrep2 += "Content-type: text/html\r\n";
  expectedrep2 += "Content-length: 28\r\n\r\n";
  expectedrep2 += "This is the second response.";

  // Generate the responses, test them.
  unsigned char buf1[1024] = { 0 };
  ASSERT_TRUE(hc.WriteResponse(rep1));
  ASSERT_EQ(static_cast<int>(72), WrappedRead(spair[1], buf1, 1024));
  ASSERT_EQ(expectedrep1, (const char *) buf1);

  unsigned char buf2[1024] = { 0 };
  ASSERT_TRUE(hc.WriteResponse(rep2));
  ASSERT_EQ(static_cast<int>(92), WrappedRead(spair[1], buf2, 1024));
  ASSERT_EQ(expectedrep2, (const char *) buf2);

  // Clean up.
  close(spair[0]);
  close(spair[1]);
  HW4Environment::AddPoints(10);
}

}  // namespace hw4
