#ifndef READ_FRAMES
#define READ_FRAMES

#include <string>
#include <vector>
#include <algorithm>

#include <dirent.h>

std::vector<std::string> read_frames(const std::string& dir_name){
  std::vector<std::string> frames;
  DIR* d;
  struct dirent *dir;
  d = opendir(dir_name.c_str());
  if (d) {
    while ((dir = readdir(d)) != NULL) {
      std::string file(dir->d_name);
      if(file != "." && file != "..")
        frames.push_back(std::string(dir_name).append(file));
    }
  }
  closedir(d);
  std::sort(frames.begin(),frames.end(),std::greater<std::string>());
  return frames;
}
    
#endif