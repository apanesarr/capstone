This document has the standards for the communication protocol
between the Server, Master, and Insect

# Communication Protocol

Each message will follow the following overall format

```json
{
    "MessageId"     : Int,      // Unique message id
    "RecipientId"   : Int,      // Usually the insect id
    "MessageType"   : String,
    "Data"          : Object    // Structure depends on the MessageType
}
```

| | |t
|--|--|
| MessageId     | Unique identifier for each message - used to ensure communication reliability |
| RecipientId   | ID of who the message is meant for. Usually the Insect ID |
| MessageType   | See Communication Methods section for each MessageType | 
| Data          | Message body |

# Communication Methods

The following represents the 'Data' section of each message.

## Request measurement reading

```json
MessageType: "RequestMeasurement"
```

Challenge:

```json
{}
```

Response:

```json
{
    "Temperature"   : Float,    // In degrees Celcius
    "Humidity"      : Float     // Percentage humidity (0 < x < 100)
}
```

## Set motion state

```json
MessageType: "SetState"
```

Challenge:

```json
{
    "State"     : String,  // FORWARD, REVERSE, LEFT, RIGHT, STOP
    "Angles"    : Float,   // Only if State is LEFT or RIGHT
    "Distance"  : Float,   // Only if State is FORWARD
}
```

Response:

```json
{}
```

## Get location

```json
MessageType: "GetLocation"
```

Challenge:

```json
{}
```

Response:

```json
{
    "X"       : Float,
    "Y"       : Float,
    "Angle"   : Float
}
```

## Arrived at Location

MessageId is not necessary for this one

```json
MessageType: "Arrived"
```

```json
{
    "X"     : Float,
    "Y"     : Float,
    "Angle" : Float
}
```

## Send Failed

```json
{
    "Error":        : String
}
```

