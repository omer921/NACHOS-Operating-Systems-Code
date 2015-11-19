#ifndef REQUEST_H
#define REQUEST_H

class Request {
public:
  Request();
  ~Request();
  void SetName(char* name);
  void SetId(int id);
  const char* GetName() const;
  int GetId() const;
private:
  char* _name;
  int _id;
};

#endif
