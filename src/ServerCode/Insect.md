# Insect Pairing

In the initial pairing period, the server is turned on first and pairing is
initialized. When each arduino is powered on, it should send a message
with the format:

```
"recipient": "PAIR"
"id": 1
"colorRange": "(0, 255, 0), (40, 250, 40)"
```
