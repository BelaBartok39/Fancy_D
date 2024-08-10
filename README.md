# Fancy Directory Sort (C Version)

## Overview

Fancy Directory Sort is a powerful and customizable file organization tool written in C. It automatically categorizes files in a directory based on their extensions, allowing for efficient file management and improved directory structure.

## Features

- Automatic file sorting based on file extensions
- Customizable category configurations using JSON files
- Ability to add new file extensions to existing categories
- Support for organizing the current directory or a specified directory
- Option for extreme sorting (subcategories based on specific file extensions)

## Installation

### Prerequisites

- GCC compiler
- Make utility
- cJSON library

### Steps

1. Clone the repository:

git clone https://github.com/yourusername/fancy-directory-sort.git
cd fancy-directory-sort

2. Compile the program:

make

3. Install the program:

make install

This will install the `fancyC` executable in your `~/bin` directory. Ensure that `~/bin` is in your PATH.

## Usage

### Basic Usage
To organize files in the current directory:

fancyC

To organize files in a specific directory:

fancyC /path/to/directory

### Adding New Extensions
To add a new file extension to a category:

fancyC --add .xyz newcategory

### Options

- `--extreme`: Enable extreme sorting by individual extensions
- `--add EXT CATEGORY`: Add a file extension to a category
- `-v, --verbose`: Enable verbose logging
- `--uninstall`: Uninstall Fancy Directory Sort
- `-h, --help`: Display help message

## Configuration
Fancy Directory Sort uses JSON configuration files located in `~/.fancyC/` to define file categories. Each category has its own config file (e.g., `document_config.json`, `image_config.json`).

Example configuration file (`image_config.json`):
```json
{
  ".jpg": "JPEG",
  ".png": "PNG",
  ".gif": "GIF"
}
```
You can modify these files to customize your sorting categories and extensions.

## Uninstall
To uninstall Fancy Directory Sort:

make uninstall

## Troubleshooting
If you encounter any issues:

Ensure all dependencies are correctly installed.
Check that the configuration files in ~/.fancyC/ are properly formatted.
Use the -v option for verbose output to diagnose issues.

Contributing
Contributions to Fancy Directory Sort are welcome! Please feel free to submit pull requests, create issues or spread the word.
License
