#include "request.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "utility.h"

#define MAX_LEN 255

Request::Request() {
  _name = (char*)malloc(MAX_LEN + 1);
  ASSERT(_name != NULL);
  _name[0] = '\0';
}

Request::~Request() {
  free(_name);
}

void
Request::SetName(char* name) {
  if (strlen(_name) > MAX_LEN) {
    fprintf(stderr, "%s is too long!\n", name);
    ASSERT(false);
  }
  strncpy(_name, name, strlen(name));
}

void
Request::SetId(int id) {
  _id = id;
}

const char*
Request::GetName() const {
  return _name;
}

int
Request::GetId() const {
  return _id;
}
