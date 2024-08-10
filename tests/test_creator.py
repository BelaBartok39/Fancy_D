import random
import string
import os


def create_test_files(directory, num_files=50):
    extensions = {
        'archive': ['.zip', '.rar', '.7z', '.tar', '.gz'],
        'audio': ['.mp3', '.wav', '.aac', '.flac', '.ogg'],
        'document': ['.doc', '.docx', '.pdf', '.txt', '.rtf'],
        'image': ['.jpg', '.png', '.gif', '.bmp', '.tiff'],
        'language': ['.py', '.java', '.cpp', '.js', '.html'],
        'video': ['.mp4', '.avi', '.mkv', '.mov', '.wmv']
    }
    
    os.makedirs(directory, exist_ok=True)
    
    for _ in range(num_files):
        category = random.choice(list(extensions.keys()))
        ext = random.choice(extensions[category])
        filename = ''.join(random.choices(string.ascii_lowercase, k=8)) + ext
        filepath = os.path.join(directory, filename)
        
        with open(filepath, 'w') as f:
            f.write(f"This is a test {category} file.")
    
    print(f"{num_files} test files created in {directory}")

def main():
    test_dir = 'Test'
    create_test_files(test_dir)

if __name__ == '__main__':
    main()
