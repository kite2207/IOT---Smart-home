import sys
import json
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

def main():
    if sys.platform == "win32":
        import msvcrt
        msvcrt.setmode(sys.stdin.fileno(), os.O_BINARY)

    try:
        # Node-RED exec node (addpay: true) passes payload via stdin if useSpawn is false,
        # or as an argument. Let's try reading from sys.stdin or sys.argv[1].
        input_data = ""
        if not sys.stdin.isatty():
            input_data = sys.stdin.read()
        
        if not input_data and len(sys.argv) > 1:
            input_data = sys.argv[1]

        data = json.loads(input_data)
        
        sender = data.get('smtp_user')
        password = data.get('smtp_pass')
        receiver = data.get('to')
        subject = data.get('subject')
        body = data.get('body')

        if not sender or not password:
            print("Missing SMTP credentials")
            sys.exit(1)

        msg = MIMEMultipart()
        msg['From'] = sender
        msg['To'] = receiver
        msg['Subject'] = subject
        msg.attach(MIMEText(body, 'plain', 'utf-8'))

        server = smtplib.SMTP_SSL('smtp.gmail.com', 465)
        server.login(sender, password)
        server.send_message(msg)
        server.quit()
        print("Email sent successfully")
    except Exception as e:
        print(f"Failed to send email: {e}")
        sys.exit(1)

if __name__ == '__main__':
    import os
    main()
