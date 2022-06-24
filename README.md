# spammer9k aka Newsletter daemon

This project started as a way to send thousands of email daily to newsletter subscribers,
alerting them when there was new content. It has multiple configurable send addressess
to avoid spam filters and configurable time delay between email per provider per address.
Email body gets generated with inja, which is a templating engine based on nlohmann's json
implementation for c++. Then it uses Microsoft's cpprestsdk to provide endpoints used
to push emails in the send queue and to get status about current job. Emails are sent
to the mail daemon using libcurl

# Ubuntu / Debian

### Dependencies
```
apt install cmake libcpprest-dev libcurl4-openssl-dev
```

### Project build
```
mkdir build
cd build
cmake ..
make
```
