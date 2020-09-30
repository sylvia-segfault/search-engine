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

#include <stdint.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include <vector>

#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpConnection.h"

#define BUF 1024

using std::map;
using std::string;
using std::vector;
using std::endl;
using std::cout;

namespace hw4 {

static const char *kHeaderEnd = "\r\n\r\n";
static const int kHeaderEndLen = 4;

bool HttpConnection::GetNextRequest(HttpRequest *request) {
  // Use "WrappedRead" to read data into the buffer_
  // instance variable.  Keep reading data until either the
  // connection drops or you see a "\r\n\r\n" that demarcates
  // the end of the request header.
  //
  // Once you've seen the request header, use ParseRequest()
  // to parse the header into the *request argument.
  //
  // Very tricky part:  clients can send back-to-back requests
  // on the same socket.  So, you need to preserve everything
  // after the "\r\n\r\n" in buffer_ for the next time the
  // caller invokes GetNextRequest()!

  // STEP 1:

  // If "\r\n\r\n" is found, we can parse a single
  // full request immediately.
  size_t pos = buffer_.find(kHeaderEnd);
  if (pos != string::npos) {
    // Parse the request into the return parameter
    *request = ParseRequest(buffer_.substr(0, pos));

  // Otherwise, keep reading data from the file
  // descriptor associated with the client.
  } else {
    unsigned char readbuf[BUF];
    while (1) {
      int numread = WrappedRead(fd_, readbuf, BUF);
      // If numread is negative, that means we can't
      // read from the file. If numread is 0, it is
      // also impossible to find a single request,
      // nor could we find "\r\n\r\n". Thus, return
      // false.
      if (numread <= 0)
        return false;

      // Append the data we just read to the buffer.
      buffer_ += string(reinterpret_cast<char *>(readbuf), numread);

      // Check if we have a single full request yet
      // at this point by finding "\r\n\r\n".
      // If so, we are able to parse the request.
      pos = buffer_.find(kHeaderEnd);
      if (pos != string::npos) {
        *request = ParseRequest(buffer_.substr(0, pos));
        break;
      }
    }
  }

  // Preserve everything after the "\r\n\r\n" in
  // buffer_ for the next time the caller invokes
  // GetNextRequest()!
  buffer_ = buffer_.substr(pos + kHeaderEndLen);
  return true;  // You may want to change this.
}

bool HttpConnection::WriteResponse(const HttpResponse &response) {
  string str = response.GenerateResponseString();
  int res = WrappedWrite(fd_,
                         (unsigned char *) str.c_str(),
                         str.length());
  if (res != static_cast<int>(str.length()))
    return false;
  return true;
}

HttpRequest HttpConnection::ParseRequest(const string &request) {
  HttpRequest req("/");  // by default, get "/".

  // Split the request into lines.  Extract the URI from the first line
  // and store it in req.URI.  For each additional "line beyond the
  // first, extract out the header name and value and store them in
  // req.headers_ (i.e., HttpRequest::AddHeader).  You should look
  // at HttpRequest.h for details about the HTTP header format that
  // you need to parse.
  //
  // You'll probably want to look up boost functions for (a) splitting
  // a string into lines on a "\r\n" delimiter, (b) trimming
  // whitespace from the end of a string, and (c) converting a string
  // to lowercase.

  // STEP 2:

  // Split the given result into lines on a "\r\n" delimiter
  vector<string> output;
  boost::split(output, request, boost::is_any_of("\r\n"),
               boost::token_compress_on);
  // Trim the trailing whilespace from each line of the request
  for (size_t i = 0; i < output.size(); i++) {
    boost::trim(output[i]);
  }

  // Extract the URI from the first line and store it
  // in req.URI
  vector<string> first_line;
  boost::split(first_line, output[0], boost::is_any_of(" "));
  req.set_uri(first_line[1]);

  // For each additional line beyond the first,
  // extract out the header name and value and
  // store them in req.headers_
  for (size_t i = 1; i < output.size(); i++) {
    vector<string> tokens;
    boost::split(tokens, output[i], boost::is_any_of(": "),
                 boost::token_compress_on);
    boost::to_lower(tokens[0]);
    req.AddHeader(tokens[0], tokens[1]);
  }
  return req;
}

}  // namespace hw4
