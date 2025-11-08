# Multi Env OS

Multi Env OS is an operating system designed to provide a versatile programming environment that includes support for Python, C++, and Node.js by default. This project aims to create a user-friendly interface for developers and users to easily access various programming tools and applications.

## Features

- **Kernel**: The core of the operating system that initializes the system and manages resources.
- **Web Browser**: A simple browser application for browsing the internet and downloading content.
- **Text Editor**: An application for creating and editing text files.
- **Terminal**: A command-line interface for executing commands and interacting with the OS.
- **Python Environment**: A setup for running Python scripts and applications.
- **C++ Environment**: A setup for compiling and running C++ programs.
- **Node.js Environment**: A setup for running JavaScript applications on the server side.
- **Device Drivers**: Manage hardware interactions and facilitate communication between the OS and hardware components.

## Project Structure

```
multi-env-os
├── src
│   ├── kernel
│   │   └── main.c
│   ├── apps
│   │   ├── browser
│   │   │   └── browser.c
│   │   ├── editor
│   │   │   └── editor.c
│   │   ├── terminal
│   │   │   └── terminal.c
│   │   ├── python_env
│   │   │   └── python_env.c
│   │   ├── cpp_env
│   │   │   └── cpp_env.cpp
│   │   └── nodejs_env
│   │       └── nodejs_env.js
│   ├── drivers
│   │   └── driver.c
│   └── include
│       └── os.h
├── build
│   └── Makefile
├── README.md
```

## Setup Instructions

1. Clone the repository to your local machine.
2. Navigate to the `build` directory.
3. Run `make` to compile the project.
4. Follow the instructions in the terminal to run the operating system.

## Usage Guidelines

- Upon startup, the operating system will present a menu with options to launch the browser, text editor, terminal, or programming environments.
- Use the terminal to execute commands and manage files within the OS.
- The browser allows users to navigate the web and download files directly to the system.
- Users can switch to the Python, C++, or Node.js environments to run their respective applications.

## Contributing

Contributions are welcome! Please submit a pull request or open an issue for any suggestions or improvements.