# Wireless transfer protocol specification

## Tech stack:
Currently, we're only depening on ESP-NOW for the underlying wireless link, the rest of the protocol consists of carefully crafted C structs.

- ESP-NOW

## In brief
The controller broadcasts updates to nearby recievers an abitrary amount of times per second, consisting of all inputs in a sigular packet. Reciever acknowledgements are currently not implemented.

## The deets
Communication between remote and reciever happens using the `TransitStruct`, the remote sends this packet many times per second, precice timings havn't been defined yet.
```cpp
struct TransitStruct {
  uint8_t throttle;
  uint8_t steer;
};
```

Transfer happens using ESP-NOW broadcast packets on WiFi channel 6, device pairing is planned but not yet implemented.

In terms of the reciever, upon recieving this packet, values are immediately written to PWM channels and servo driver, and then the packet is discarded without further notice.