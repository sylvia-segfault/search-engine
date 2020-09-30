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

#include <boost/algorithm/string.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include <sstream>

#include "./FileReader.h"
#include "./HttpConnection.h"
#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpServer.h"
#include "./libhw3/QueryProcessor.h"

using std::cerr;
using std::cout;
using std::endl;
using std::list;
using std::map;
using std::string;
using std::stringstream;
using std::unique_ptr;
using std::vector;

namespace hw4 {
///////////////////////////////////////////////////////////////////////////////
// Constants, internal helper functions
///////////////////////////////////////////////////////////////////////////////
static const char *kThreegleStr =
  "<html><head><title>333gle</title></head>\n"
  "<body>\n"
  "<center style=\"font-size:500%;\">\n"
  "<span style=\"position:relative;bottom:-0.33em;color:orange;\">3</span>"
    "<span style=\"color:red;\">3</span>"
    "<span style=\"color:gold;\">3</span>"
    "<span style=\"color:blue;\">g</span>"
    "<span style=\"color:green;\">l</span>"
    "<span style=\"color:red;\">e</span>\n"
  "</center>\n"
  "<p>\n"
  "<div style=\"height:20px;\"></div>\n"
  "<center>\n"
  "<form action=\"/query\" method=\"get\">\n"
  "<input type=\"text\" size=30 name=\"terms\" />\n"
  "<input type=\"submit\" value=\"Search\" />\n"
  "</form>\n"
  "</center><p>\n";

// static
const int HttpServer::kNumThreads = 100;

// This is the function that threads are dispatched into
// in order to process new client connections.
void HttpServer_ThrFn(ThreadPool::Task *t);

// Given a request, produce a response.
HttpResponse ProcessRequest(const HttpRequest &req,
                            const string &basedir,
                            const list<string> *indices);

// Process a file request.
HttpResponse ProcessFileRequest(const string &uri,
                                const string &basedir);

// Process a query request.
HttpResponse ProcessQueryRequest(const string &uri,
                                 const list<string> *indices);


///////////////////////////////////////////////////////////////////////////////
// HttpServer
///////////////////////////////////////////////////////////////////////////////
bool HttpServer::Run(void) {
  // Create the server listening socket.
  int listen_fd;
  cout << "  creating and binding the listening socket..." << endl;
  if (!ss_.BindAndListen(AF_INET6, &listen_fd)) {
    cerr << endl << "Couldn't bind to the listening socket." << endl;
    return false;
  }

  // Spin, accepting connections and dispatching them.  Use a
  // threadpool to dispatch connections into their own thread.
  cout << "  accepting connections..." << endl << endl;
  ThreadPool tp(kNumThreads);
  while (1) {
    HttpServerTask *hst = new HttpServerTask(HttpServer_ThrFn);
    hst->basedir = staticfileDirpath_;
    hst->indices = &indices_;
    if (!ss_.Accept(&hst->client_fd,
                    &hst->caddr,
                    &hst->cport,
                    &hst->cdns,
                    &hst->saddr,
                    &hst->sdns)) {
      // The accept failed for some reason, so quit out of the server.
      // (Will happen when kill command is used to shut down the server.)
      break;
    }
    // The accept succeeded; dispatch it.
    tp.Dispatch(hst);
  }
  return true;
}

void HttpServer_ThrFn(ThreadPool::Task *t) {
  // Cast back our HttpServerTask structure with all of our new
  // client's information in it.
  unique_ptr<HttpServerTask> hst(static_cast<HttpServerTask *>(t));
  cout << "  client " << hst->cdns << ":" << hst->cport << " "
       << "(IP address " << hst->caddr << ")" << " connected." << endl;

  // Read in the next request, process it, write the response.

  // Use the HttpConnection class to read and process the next
  // request from our current client, then write out our response.  If
  // the client sends a "Connection: close\r\n" header, then shut down
  // the connection -- we're done.
  //
  // Hint: the client can make multiple requests on our single connection,
  // so we should keep the connection open between requests rather than
  // creating/destroying the same connection repeatedly.

  // STEP 1:
  // Start a http connection for our current client
  HttpConnection htc(hst->client_fd);
  bool done = false;
  while (!done) {
    // Get the request from the client.
    // If it failed, exit the while loop
    // immediately.
    HttpRequest request;
    if (!htc.GetNextRequest(&request)) {
      break;
    }

    // Process the next request from our client
    HttpResponse response = ProcessRequest(request,
                                           hst->basedir,
                                           hst->indices);

    // Write out our response.
    // If it failed, set done to true
    // to exit out of the while loop.
    if (!htc.WriteResponse(response)) {
      done = true;
    }

    // If the client sends a "Connection: close" header,
    // shut down the connection.
    if (request.GetHeaderValue("connection") == "close") {
      break;
    }
  }
}

HttpResponse ProcessRequest(const HttpRequest &req,
                            const string &basedir,
                            const list<string> *indices) {
  // Is the user asking for a static file?
  if (req.uri().substr(0, 8) == "/static/") {
    return ProcessFileRequest(req.uri(), basedir);
  }

  // The user must be asking for a query.
  return ProcessQueryRequest(req.uri(), indices);
}

HttpResponse ProcessFileRequest(const string &uri,
                                const string &basedir) {
  // The response we'll build up.
  HttpResponse ret;

  // Steps to follow:
  //  - use the URLParser class to figure out what filename
  //    the user is asking for.
  //
  //  - use the FileReader class to read the file into memory
  //
  //  - copy the file content into the ret.body
  //
  //  - depending on the file name suffix, set the response
  //    Content-type header as appropriate, e.g.,:
  //      --> for ".html" or ".htm", set to "text/html"
  //      --> for ".jpeg" or ".jpg", set to "image/jpeg"
  //      --> for ".png", set to "image/png"
  //      etc.
  //
  // be sure to set the response code, protocol, and message
  // in the HttpResponse as well.
  string fname = "";

  // STEP 2:
  // Figure out what filename the user is asking for
  URLParser url_parser;
  url_parser.Parse(uri);
  // Since the parsed file name starts with
  // /static/, we need to ignore it.
  fname = url_parser.path().substr(8);

  // Read the file into memory
  string content;
  FileReader file_reader(basedir, fname);
  bool read = file_reader.ReadFile(&content);

  // The file can be read, and thus we can
  // find the suffix of the file name to
  // set the response Content-type header
  // as appropriate.}
  if (read) {
    // copy the file content into the ret.body
    ret.AppendToBody(content);
    // Find the last occurence of . in the file
    // name because we only care about the last
    // four characters.
    string suffix = fname.substr(fname.rfind("."));

    // set the response Content-type header
    // as appropriate.
    if (suffix == ".html" || suffix == ".htm") {
      ret.set_content_type("text/html");
    } else if (suffix == ".jpeg" || suffix == ".jpg") {
      ret.set_content_type("image/jpeg");
    } else if (suffix == ".png") {
      ret.set_content_type("image/png");
    } else if (suffix == ".css") {
      ret.set_content_type("text/css");
    } else if (suffix == ".gif") {
      ret.set_content_type("image/gif");
    } else if (suffix == ".tiff") {
      ret.set_content_type("image/tiff");
    } else if (suffix == ".xml") {
      ret.set_content_type("text/xml");
    } else {
      ret.set_content_type("text/plain");
    }

    // Set the response code, protocol, and message
    // in the returned HttpResponse.
    ret.set_protocol("HTTP/1.1");
    ret.set_response_code(200);
    ret.set_message("OK");
    return ret;
  }

  // If you couldn't find the file, return an HTTP 404 error.
  ret.set_protocol("HTTP/1.1");
  ret.set_response_code(404);
  ret.set_message("Not Found");
  ret.AppendToBody("<html><body>Couldn't find file \""
                   + EscapeHTML(fname)
                   + "\"</body></html>");
  return ret;
}

HttpResponse ProcessQueryRequest(const string &uri,
                                 const list<string> *indices) {
  // The response we're building up.
  HttpResponse ret;

  // Your job here is to figure out how to present the user with
  // the same query interface as our solution_binaries/http333d server.
  // A couple of notes:
  //
  //  - no matter what, you need to present the 333gle logo and the
  //    search box/button
  //
  //  - if the user had previously typed in a search query, you also
  //    need to display the search results.
  //
  //  - you'll want to use the URLParser to parse the uri and extract
  //    search terms from a typed-in search query.  convert them
  //    to lower case.
  //
  //  - you'll want to create and use a hw3::QueryProcessor to process
  //    the query against the search indices
  //
  //  - in your generated search results, see if you can figure out
  //    how to hyperlink results to the file contents, like we did
  //    in our solution_binaries/http333d.

  // STEP 3:

  // To present the 333gle logo and the search box/button, we need
  // to save kThreegleStr into ret.body
  ret.AppendToBody(string(kThreegleStr, strlen(kThreegleStr)));

  // Parse the URI and extract search terms from a typed-in search
  // query and convert them to lower case.
  URLParser url_parser;
  url_parser.Parse(uri);
  map<string, string> args = url_parser.args();
  string query = args["terms"];
  boost::to_lower(query);
  boost::trim(query);

  // Extract the query words input by the customer
  // from the URI
  if (uri.find("query?terms=") != string::npos) {
    // We found some input from the user

    // Construct a query processor to process
    // the query words
    hw3::QueryProcessor qp(*indices, false);

    // Create a vector of query words so that
    // the query processor can process them
    vector<string> queries;
    boost::split(queries, query, boost::is_any_of(" "));

    // Process the query words
    vector<hw3::QueryProcessor::QueryResult> res = qp.ProcessQuery(queries);

    ret.AppendToBody("<p><br>\n");
    int res_size = res.size();
    // If no matching document can be found, present
    // a message which indicates that on the web page.
    if (res_size == 0) {
      ret.AppendToBody("No results found for <b>");
      ret.AppendToBody(EscapeHTML(query));
      ret.AppendToBody("</b>\n<p>");
      ret.AppendToBody("\n");

    // If matching documents were found, present them on
    // the web page using hyperlinks.
    } else {
      // Use stringstream to save the string representation
      // which indicates the number of results found for
      // the given query words
      stringstream ss;
      if (res_size == 1) {
        ss << "1 result found for <b>";
      } else {
        ss << res_size << " results found for <b>";
      }
      ret.AppendToBody(ss.str());
      ret.AppendToBody(EscapeHTML(query));
      ret.AppendToBody("</b>\n<p>\n<ul>\n");

      // Present the results of all the matching documents
      // for the given query words, using hyperlinks.
      for (int i = 0; i < res_size; i++) {
        // HTML tag for a hyperlink
        ret.AppendToBody(" <li> <a href=\"");

        // If the document name does not start with "http://",
        // it is a static file, and thus append the format for
        // a static file, which starts with "/static/" followed
        // by the name of the matching document.
        string documentName = res[i].documentName;
        if (documentName.substr(0, 7) != "http://") {
          ret.AppendToBody("/static/");
          ret.AppendToBody(documentName);
        // If it is another link which starts with "http://",
        // escape any special HTML character for security
        // reason, and present the full link.
        } else {
          ret.AppendToBody(EscapeHTML(documentName));
        }
        ret.AppendToBody("\">");
        ret.AppendToBody(documentName);
        ret.AppendToBody("</a> [");

        // Clear the stringstream that was used previously,
        // and store the rank for this matching document.
        // Append the rank to the body of the HTML code.
        ss.str("");
        ss << res[i].rank;
        ret.AppendToBody(ss.str());
        ret.AppendToBody("]<br>\n");
      }
      ret.AppendToBody("</ul>\n");
    }
  }
  ret.AppendToBody("</body>\n</html>\n");

  // Set the response code, protocol, and message
  // in the returned HttpResponse.
  ret.set_protocol("HTTP/1.1");
  ret.set_response_code(200);
  ret.set_message("OK");
  return ret;
}

}  // namespace hw4
