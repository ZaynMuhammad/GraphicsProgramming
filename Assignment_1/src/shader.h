#pragma once
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>

class Shader {
    private:
        std::string filename;
        std::string contents;

    public:
        Shader(){};
    
    // Helper method for loading glsl files
    Shader(std::string _filename) {
        filename = _filename;
        // Load file
        std::ifstream fs(filename);
        std::string str = "";
        std::string line;
        std::cout << "Loading Shader: " << filename << std::endl;   

        if (fs.is_open()) {
            while (getline(fs, line)) {
                str = str + line + "\n";
            }
            fs.close();
        } else {
            std::cout << "Error reading file: " << filename << std::endl;
            // Terminate program if can't read file
            exit(1);
        }
        contents = str;
    } 

std::string &read() { return contents; }

};