# Modules

## GPS

Air530Z GNSS module communication

Each PCAS command follows the NMEA-style format:

```$PCASxx,...*CS<CR><LF>```

- `xx` is the command number.
- `*CS` is the checksum, calculated as the XOR of all characters between `$` and `*`.
- `<CR><LF>` indicates carriage return and line feed.


### PCAS00 — Save Configuration to FLASH
- **Purpose**: Persist settings across power cycles.
- **Format**: `$PCAS00*CS`

---

### PCAS01 — Set Baud Rate
- **Format**: `$PCAS01,br*CS`
- **Options**:
  - `0` = 4800 bps
  - `1` = 9600 bps
  - `2` = 19200 bps
  - `3` = 38400 bps
  - `4` = 57600 bps
  - `5` = 115200 bps

---

### PCAS02 — Set Positioning Update Rate
- **Format**: `$PCAS02,fixInt*CS`
- **Values (ms)**:
  - `1000` = 1Hz
  - `500` = 2Hz
  - `250` = 4Hz
  - `200` = 5Hz
  - `100` = 10Hz

---

### PCAS03 — Set Output of NMEA Sentences
- **Format**:  
  ```text
  $PCAS03,nGGA,nGLL,nGSA,nGSV,nRMC,nVTG,nZDA,nANT,nDHV,nLPS,res1,res2,nUTC,nGST,res3,res4,res5,nTIM*CS
  ```
  Each field:

   - Value 0 = Do not output

   - Value 1-9 = Output once every 1–9 position fixes

   - Empty ('') = Keep current setting
    
    | Position | Field       | Description                   |
    | -------- | ----------- | ----------------------------- |
    | 1        | `nGGA`      | GPS fix data                  |
    | 2        | `nGLL`      | Geographic position           |
    | 3        | `nGSA`      | DOP & active satellites       |
    | 4        | `nGSV`      | Satellites in view            |
    | 5        | `nRMC`      | Recommended minimum GNSS data |
    | 6        | `nVTG`      | Ground speed & heading        |
    | 7        | `nZDA`      | Date & time                   |
    | 8        | `nANT`      | Active antenna info           |
    | 9        | `nDHV`      | Heading/velocity              |
    | 10       | `nLPS`      | Local position                |
    | 11       | `res1`      | Reserved                      |
    | 12       | `res2`      | Reserved                      |
    | 13       | `nUTC`      | UTC time                      |
    | 14       | `nGST`      | Pseudorange error stats       |
    | 15–17    | `res3–res5` | Reserved                      |
    | 18       | `nTIM`      | Time info                     |




### PCAS04 — Set Working Satellite System
- **Format**: `$PCAS04,mode*CS`
- **Options**:
- `1` = GPS only  
- `2` = BDS only  
- `3` = GPS + BDS  
- `4` = GLONASS only  
- `5` = GPS + GLONASS  
- `6` = BDS + GLONASS  
- `7` = GPS + BDS + GLONASS  

---

### PCAS05 — Select NMEA Protocol Version
- **Format**: `$PCAS05,ver*CS`
- **Options**:
- `2` = GPS NMEA 2.2  
- `5` = NMEA 4.1  
- `9` = BDS/GPS dual-mode (NMEA 2.3+ and 4.0 compatible)

---

### PCAS06 — Query Product Information
- **Format**: `$PCAS06,info*CS`
- **Options**:
- `0` = Firmware version  
- `1` = Hardware model and serial number  
- `2` = Working mode of the receiver  
- `3` = Product customer number  
- `5` = Upgrade code information

---

### PCAS10 — Restart Receiver
- **Format**: `$PCAS10,rs*CS`
- **Options**:
- `0` = Hot Start  
- `1` = Warm Start  
- `2` = Cold Start  
- `3` = Factory Reset  

---

### PCAS12 — Standby Mode Control
- **Format**: `$PCAS12,stdbysec*CS`
- **Purpose**: Puts receiver into standby mode for `stdbysec` seconds (max: `65535`)

---

### PCAS15 — Satellite System Control
- **Format**: `$PCAS15,SYS_ID,SV_MASK*CS`
- **Parameters**:
- `SYS_ID`:
  - `2` = BeiDou 1–32  
  - `3` = BeiDou 33–64  
  - `4` = SBAS (PRN 120–138)  
  - `5` = QZSS (PRN 193–199)
- `SV_MASK`: Hex bitmask (8 hex digits = 32 bits = 32 satellites)  
  - `1` = Enable satellite  
  - `0` = Disable satellite

- **Examples**:
- `$PCAS15,2,FFFFFFFF*xx` → Enable all BeiDou 1–32  
- `$PCAS15,2,FFFFFFE0*xx` → Disable BeiDou 1–5  

---

### PCAS20 — Initiate Online Upgrade
- **Format**: `$PCAS20*CS`
- **Purpose**: Start firmware upgrade process via connected interface

---

### PCAS60 — Receiver Time Output
- **Format**:  

$PCAS60,UTCtime,ddmmyyyy,wn,tow,timevalid,leaps,leapsValid*CS

Always show details


- **Field Descriptions**:
- `UTCtime` – e.g., `091222.000`  
- `ddmmyyyy` – e.g., `23122019`  
- `wn` – GPS week number  
- `tow` – Time of week in seconds  
- `timevalid` – `1` = Valid, `0` = Invalid  
- `leaps` – Leap seconds (GPS-UTC)  
- `leapsValid` – `1` = Valid, `0` = Invalid

- **Example**:
`$PCAS60,092011.000,23122019,2085,120029,1,,0*33`

# Core

```
┌────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                                                                                    │
│                                                                                                    │
│    ┌─────────────────────────┐                                                                     │
│    │                         │                                                                     │
│    │   radio communication   │                                                                     │
│    │                         │                                                                     │
│    └─────────────────────────┘                                                                     │
│           ▲                                                                                        │
│           │                                                                                        │
│           │                                                                                        │
│           ▼                                                                                        │
│    ┌────────────────┐    ┌────────────────┐    ┌─────────────────────────┐                         │
│    │                │    │                │    │                         │                         │
│    │                │    │                ├───►│  barometer              │                         │
│    │                │    │                │    │                         │                         │
│    │ main loop      │    │ second loop    │    └─────────────────────────┘                         │
│    │                │◄──►│                │    ┌─────────────────────────┐                         │
│    │                │    │                │    │                         │                         │
│    │                │    │                ├───►├  logging                │                         │
│    │                │    │                │    │                         │                         │
│    └────────────────┘    └────────────────┘    └─────────────────────────┘                         │
│            ▲                                                                                       │
│            │                                                                                       │
│            │                                                                                       │
│            │                                                                                       │
│            │                                                                                       │
│            │                                                                                       │
│            │                                                                second microcontroller │
└────────────┼───────────────────────────────────────────────────────────────────────────────────────┘
┌────────────┼───────────────────────────────────────────────────────────────────────────────────────┐
│            ▼                                                                                       │
│    ┌────────────────┐    ┌────────────────┐    ┌────────────────┐                                  │
│    │                │    │                │    │                │                                  │
│    │                │    │                ├───►│      gps       │                                  │
│    │                │    │                │    │                │                                  │
│    │ main loop      │    │ second loop    │    └────────────────┘                                  │
│    │                │◄──►│                │    ┌────────────────┐                                  │
│    │                │    │                │    │                │                                  │
│    │                │    │                ├───►│  acceleremoter │                                  │
│    │                │    │                │    │                │                                  │
│    └───────┬────────┘    └────────────────┘    └────────────────┘                                  │
│            │                                                                                       │
│            ▼                                                                                       │
│    ┌────────────────┐                                                                              │
│    │                │                                                                              │
│    │ system control │                                                                              │
│    │                │                                                                              │
│    └────────────────┘                                                                              │
│                                                                             first micro controller │
└────────────────────────────────────────────────────────────────────────────────────────────────────┘
```