# Fancy Directory Sort (C Version)

## Overview

Fancy Directory Sort is a powerful and customizable file organization tool written in C. It automatically categorizes files in a directory based on their extensions, allowing for efficient file management and improved directory structure.

## Features

- Automatic file sorting based on file extensions
- Customizable category configurations using JSON files
- Ability to add new file extensions to existing categories or create new categories
- Support for organizing the current directory or a specified directory
- Option for extreme sorting (subcategories based on specific file extensions)
- Handles long file paths with fallback strategies

## Installation

### Prerequisites

- GCC compiler
- Make utility
- cJSON library

### Steps

1. Clone the repository:
```bash
git clone https://github.com/BelaBartok39/Fancy_D.git
cd Fancy_D
```

2. Compile the program:
```bash
make
```

3. Install the program:
```bash
make install
```

This will install the `fancyD` executable in your `~/bin` directory. Ensure that `~/bin` is in your PATH.

## Usage

### Basic Usage
To organize files in the current directory:
```bash
fancyD
```

To organize files in a specific directory:
```bash
fancyD /path/to/directory
```

### Adding New Extensions
To add a new file extension to a category:
```bash
fancyD --add .xyz NewCategory
```
If the extension already exists in another category, you'll be prompted to move it.

### Resetting the configuration
To reset the configuration files:
```bash
fancyD --reset
```
This will delete all existing configuration files.

### Creating Default Categories
To create default category configurations:
```bash
fancyD --default
```

### Options

- `--add EXT CATEGORY`: Add a file extension to a category
- `--default`: Create default categories
- `--reset`: Reset all configurations
- `-h, --help`: Display help message

## Configuration
Fancy Directory Sort uses JSON configuration files located in `~/.fancyD/` to define file categories. Each category has its own config file (e.g., `document_config.json`, `image_config.json`).

Example configuration file (`image_config.json`):
```json
{
  ".jpg": "JPEG",
  ".png": "PNG",
  ".gif": "GIF"
}
```
You can modify these files to customize your sorting categories and extensions.

## Troubleshooting
If you encounter any issues:

- Ensure all dependencies are correctly installed.
- Check that the configuration files in ~/.fancyD/ are properly formatted.
- If you encounter issues with long file paths, the program will attempt to use a fallback strategy.

## Contributing
Contributions to Fancy Directory Sort are welcome! Please feel free to submit pull requests, create issues or spread the word.

## License
This project is licensed under the MIT License. See the LICENSE file for details.
