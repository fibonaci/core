// Env.cpp
#include "Env.h"

#include "InputXML.h"
#include "Logger.h"

#include <sys/stat.h>
#include <iostream>
#include <string>
#include <cstring>
#include <utility>

using namespace std;

boost::filesystem::path Env::path_from_cwd_to_cyclus_;
boost::filesystem::path Env::cwd_ = boost::filesystem::current_path();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// note that this is not used - Env is a pure static class

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Env::pathBase(string path) {
  string base;
  int index;

  index = path.rfind("/");
  base = path.substr(0, index);
  return base;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Env::getCyclusPath() {
  // return the join of cwd_ and rel path to cyclus
  CLOG(LEV_DEBUG4) << "Cyclus absolute path retrieved: " 
                  << cwd_ / path_from_cwd_to_cyclus_;

  boost::filesystem::path path;
  if(path_from_cwd_to_cyclus_.has_root_path()) {
    path = path_from_cwd_to_cyclus_.normalize();
  } else {
    path = (cwd_ / path_from_cwd_to_cyclus_).normalize();
  }
  return path.string();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Env::setCyclusRelPath(string path) {
  path_from_cwd_to_cyclus_ = boost::filesystem::path(path);
  CLOG(LEV_DEBUG3) << "Cyclus rel path: " << path;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string Env::checkEnv(string varname) {
  char* pVar = getenv (varname.c_str());
  if (pVar == NULL) {
    throw CycNoEnvVarException("Environment variable " + varname + " not set.");
  } else if (strlen(pVar) == 0) {
    throw CycNoEnvVarException("Environment variable " + varname
                               + " set to an empty string.");
  }
  return pVar;
}

