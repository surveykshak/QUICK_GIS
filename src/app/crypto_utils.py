import sys
import base64
import os

def xor_cipher(data: bytes, key: str) -> bytes:
    key_bytes = key.encode('utf-8')
    key_len = len(key_bytes)
    return bytes(b ^ key_bytes[i % key_len] for i, b in enumerate(data))

def encrypt_file(input_path: str, output_path: str, key: str):
    with open(input_path, 'rb') as f:
        data = f.read()
    
    # Compress, encrypt with XOR, and encode in Base64
    encrypted_data = xor_cipher(data, key)
    b64_data = base64.b64encode(encrypted_data)
    
    with open(output_path, 'wb') as f:
        f.write(b64_data)
    print(f"Encrypted {input_path} -> {output_path}")

def decrypt_file(input_path: str, output_path: str, key: str):
    with open(input_path, 'rb') as f:
        b64_data = f.read()
    
    # Decode Base64, decrypt with XOR
    encrypted_data = base64.b64decode(b64_data)
    decrypted_data = xor_cipher(encrypted_data, key)
    
    with open(output_path, 'wb') as f:
        f.write(decrypted_data)
    print(f"Decrypted {input_path} -> {output_path}")

def main():
    if len(sys.argv) != 5:
        print("Usage: python crypto_utils.py [encrypt|decrypt] [input_file] [output_file] [password]")
        sys.exit(1)
        
    mode = sys.argv[1]
    input_file = sys.argv[2]
    output_file = sys.argv[3]
    password = sys.argv[4]
    
    if mode == 'encrypt':
        encrypt_file(input_file, output_file, password)
    elif mode == 'decrypt':
        decrypt_file(input_file, output_file, password)
    else:
        print("Invalid mode. Use 'encrypt' or 'decrypt'.")
        sys.exit(1)

if __name__ == '__main__':
    main()
