''' ============================================================================= //
    Program Name: FancyD (Fancy Directory Organizer)
    Author: Nicholas D. Redmond (3A3YN1CKY)
    Date: 8/13/2024
    Description: Simple program to organize files in a directory based on their ext
    ============================================================================= '''

import random
import string
import os
import shutil

def create_test_files(directory, num_files=1000):
    extensions = {
        'archive': ['.zip', '.rar', '.7z', '.tar', '.gz'],
        'audio': ['.mp3', '.wav', '.aac', '.flac', '.ogg'],
        'document': ['.doc', '.docx', '.pdf', '.txt', '.rtf'],
        'image': ['.jpg', '.png', '.gif', '.bmp', '.tiff'],
        'language': ['.py', '.java', '.cpp', '.js', '.html'],
        'video': ['.mp4', '.avi', '.mkv', '.mov', '.wmv']
    }
    
    if os.path.exists(directory):
        shutil.rmtree(directory)
    os.makedirs(directory)
    
    for i in range(num_files):
        category = random.choice(list(extensions.keys()))
        ext = random.choice(extensions[category])
        
        # Create some files with very long names
        if i % 100 == 0:
            filename = ''.join(random.choices(string.ascii_lowercase, k=200)) + ext
        # Create some files with no extension
        elif i % 50 == 0:
            filename = ''.join(random.choices(string.ascii_lowercase, k=8))
        # Create some hidden files
        elif i % 25 == 0:
            filename = '.' + ''.join(random.choices(string.ascii_lowercase, k=8)) + ext
        else:
            filename = ''.join(random.choices(string.ascii_lowercase, k=8)) + ext
        
        filepath = os.path.join(directory, filename)
        
        with open(filepath, 'w') as f:
            f.write(f"This is a test {category} file.")
    
    # Create a few empty subdirectories
    for i in range(5):
        subdir = os.path.join(directory, f"subdir_{i}")
        os.makedirs(subdir)
    
    print(f"{num_files} test files and 5 subdirectories created in {directory}")

def main():
    test_dir = 'Test'
    create_test_files(test_dir)

if __name__ == '__main__':
    main()
