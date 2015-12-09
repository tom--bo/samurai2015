#include "samurai.hpp"
#include <thread>

CommentedIStream::CommentedIStream(istream &is):
  is(&is), commentChar('#') {
}

int CommentedIStream::get() {
  return is->get();
}

bool CommentedIStream::good() {
  return is->good();
}

void CommentedIStream::skipComments() {
  char c = get();
  while (isspace(c) || c == commentChar) {
    if (c == commentChar) {
      do {
	c = get();
      } while (c != '\n');
    } else {
      c = get();
    }
  }
  is->unget();
}

CommentedIStream& operator>>(CommentedIStream &cs, int &i) {
  cs.skipComments();
  *cs.is >> i;
  return cs;
}

CommentedIStream& operator>>(CommentedIStream& cs, string& str) {
  cs.skipComments();
  for (char c = cs.get(); c != '"'; c = cs.get()) {
    if (!isspace(c)) {
      throw ErrorReport(string("Character '") + c + 
			"' found where a string literal is expected");
    }
  }
  str = "";
  for (char c = cs.get(); c != '"'; c = cs.get()) {
    str += c;
  }
  return cs;
}
