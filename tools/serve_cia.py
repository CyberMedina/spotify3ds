#!/usr/bin/env python3
"""Serve Spotify3DS.cia locally and display a QR code for FBI Remote Install."""

import http.server
import os
import socket
import socketserver
import sys
from pathlib import Path
import qrcode

PORT = 8000
CIA_NAME = "Spotify3DS.cia"

def get_local_ip() -> str:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # Connecting to a public DNS IP doesn't actually send packets, but picks the right route/interface
        s.connect(("8.8.8.8", 80))
        ip = s.getsockname()[0]
    except Exception:
        ip = "127.0.0.1"
    finally:
        s.close()
    return ip

def main():
    root = Path(__file__).resolve().parent.parent
    cia_path = root / CIA_NAME
    if not cia_path.is_file():
        print(f"Error: {cia_path} not found!")
        sys.exit(1)

    ip = get_local_ip()
    url = f"http://{ip}:{PORT}/{CIA_NAME}"
    print(f"\n=======================================================")
    print(f" FBI Remote Install URL:")
    print(f" {url}")
    print(f"=======================================================\n")

    # Generate QR Code image
    qr = qrcode.QRCode(
        version=1,
        error_correction=qrcode.constants.ERROR_CORRECT_M,
        box_size=10,
        border=4,
    )
    qr.add_data(url)
    qr.make(fit=True)

    # Save to disk
    qr_img_path = root / "qr_fbi.png"
    img = qr.make_image(fill_color="black", back_color="white")
    img.save(str(qr_img_path))
    print(f"Saved QR code to {qr_img_path}")

    # Print ASCII QR in terminal
    try:
        qr.print_ascii(invert=True)
    except Exception:
        pass

    # Open image viewer
    try:
        os.startfile(str(qr_img_path))
        print("Opened QR Code image in default viewer.")
    except Exception:
        pass

    print(f"\nWaiting for your Nintendo 3DS (FBI) to download {CIA_NAME}...")
    print("Press Ctrl+C to stop the server when installation is complete.\n")

    os.chdir(str(root))
    handler = http.server.SimpleHTTPRequestHandler
    with socketserver.TCPServer(("", PORT), handler) as httpd:
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nServer stopped.")

if __name__ == "__main__":
    main()
