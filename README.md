# FileShare

## About
FileShare is open source command-line tool used for hosting files over network. Any type of a file is supported for hosting. Even directories can be hosted for download. Uppon request for downloading directory
server would automatically create archive version of it (.zip) and serve it. After opening server for file sharing, JSON file named `.download.json` storing information about files hosted would be created. Zip files as well as JSON would be automatically removed after closing the server so no additional info about running application will be stored on a system after exiting :)). FileShare can be also used like a simple one-liner server. 

## Setup
To use FileShare you need to build your project locally. Application can run on Linux and Windows Platforms, MacOS has not been tested. To build executable simply clone this repository using:
`git clone https://github.com/MaciejGd/FileShare.git`
or download and unzip code in wanted location. Then if you have got make installed simply type:
`make all`
After following this steps executable for your operating system should be build.

## Usage
To use FileShare:
1. Go to location you store **FileShare** executable file and create directory named **download**(Optionally you can add **FileShare.exe** to your environment path variable and use it wherever you want using command line!).
2. Put the files you want to share in a **download** dir if flag `-p` has not been used (look at **Additional options** tab for more information).
3. Launch server application.
4. From another device go to a browser and in url field type `server_device_ip:8080`, simple website should be displayed.
5. On entered website client can browse through files and download files he wants to.

### Additional options
When launching server using command line additional flags can be specified:
* -h or --help  : get information about application
* -p or --path  : after using flag specify the path to directory you want to host to a client instead of **download** dir
* -f or --serve : after this flag specify index.html file you want to serve (one-liner usage of FileShare)
