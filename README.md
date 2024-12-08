<h1 align="center">Terminal Mail</h1>

## Build Requirements (Only for Linux)

Start by downloading ncurses for console

```
sudo sudo apt install libncurses5-dev libncursesw5-dev
```

Then download mail

```
sudo apt install mailutils
```

Then create the login credentials for gmail and have them saved in sasl_passwd file using the below command

```
sudo nano /etc/postfix/sasl_passwd
```

Then add the following text with your gmail credentials

```
[smtp.gmail.com]:587 your_email@gmail.com:your_app_password
```

Use this to secure the file (No clue what this does to secure)

```
sudo chmod 600 /etc/postfix/sasl_passwd
sudo postmap /etc/postfix/sasl_passwd
sudo systemctl restart postfix
```

For me password was not working so I created an app key from gmail for the program and added that in the file instead
Also for me there was some error with ipv6 so changed in the file sysctl.conf using

```
sudo nano /etc/sysctl.conf
```

and editing to

```
net.ipv6.conf.all.disable_ipv6 = 1
net.ipv6.conf.default.disable_ipv6 = 1
net.ipv6.conf.lo.disable_ipv6 = 1
```

Then check connectivity using

```
nc -4 -zv smtp.gmail.com 587
```

Still doesn't work then use some online help

## How it works

Run this command to run the program

```
make
```

Then you will be in starting page where you can type the message you want to share then press Esc key to go to next page which will be subject page and type the subject for the mail then press Esc key again and you will be at recipient page type the email of recipient for the mail and for multiple recipients just add the email and press enter and enter another email. After that press Ctrl+r keys to send mail

Check if the mail was sent using command

```
sudo tail -f /var/log/mail.log
```
