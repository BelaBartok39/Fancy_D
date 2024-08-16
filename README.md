# Fancy Directory Sort v1.1.0 (C Version)

Fancy Directory Sort (FancyD) is a pretty darn powerful and customizable file organization tool written in C. It automatically categorizes files in a directory based on their extensions. Assign individual extensions a directory or just use our quick defaults. Fancy!

## Features ✨

- Automatic file sorting based on file extensions
- Customizable category configurations using JSON files
- Add new file extensions to existing categories
- Support for organizing the current directory or a specified directory
- Default categories for on-the-go sorting.
- Ability to reset configuration files any time you want.
- Support for a 'misc' category when no categories are defined
- List command to display current category configurations

## Installation

### Requirements

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

4. Clean up.
```bash
make clean
```

## Usage

Before running this baby on your files, feel free to use our python example scripts (normal/extreme). Either one creates a Test directory full of blank files with random extensions. You can use this to tryout Fancy and see how it easy it is before moving any of your own files. 

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
fancyD --add .xyz newcategory
```

### Creating Default Categories
To create default categories:
```bash
fancyD --default
```

### Resetting Configuration Files
To reset the configuration files:
```bash
fancyD --reset
```

### Listing Current Categories
To list all current category configurations:
```bash
fancyD --list
```

### Options

- `-a, --add EXT CATEGORY`: Add a file extension to a category
- `-h, --help`: Display help message
- `-d, --default`: Create default categories
- `-r, --reset`: Reset configuration files
- `-l, --list`: List all current category configurations

## Configuration
FancyD uses JSON configuration files located in `~/.fancyD/` to define file categories. Each category has its own config file (e.g., `document_config.json`, `image_config.json`).

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

1. Ensure all dependencies are correctly installed.
2. Check that the configuration files in `~/.fancyD/` are properly formatted JSON.
3. Use the `--reset` option to reset configuration files if they become corrupted.
4. If no categories are defined, FancyD will prompt you to create a 'misc' category for all files.
5. For long file paths, FancyD implements checks to ensure they don't exceed the maximum path length.
6. Use the `--list` option to verify your current category configurations.

## Known Issues

- The verbose logging option (`-v`) is not fully implemented.
- There may be some inconsistencies in handling the `--add` option. (Rare at the moment)

## Contributing
Contributions (and suggestions) to Fancy Directory Sort are welcome! Please feel free to submit pull requests, create issues or spread the word.

## License
This project is licensed under the MIT License. Naturally, we are not responsible for what happens to your files if software is not used as designed. Source code available for free. 
