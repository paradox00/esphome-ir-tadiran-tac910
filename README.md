# IR Tadiran TAC910 Climate

The `ir_tadiran_tac910` component allows you to control Tadiran air conditioners with TAC910 dumpers via IR.

## Configuration

```yaml
climate:
  - platform: ir_tadiran_tac910
    name: "Living Room AC"
    transmitter_id: my_transmitter
    receiver_id: my_receiver  # Optional, for receiving status from remote
```

## Supported Features

- **Temperature Range**: 16°C to 30°C
- **Fan Modes**: Auto, Low, Medium, High
- **Feedback**: Supports receiving status updates from the remote control if a receiver is configured.

## Notes

- This component currently does not advertise support for specific climate modes (Cool, Heat) in its capabilities, focusing on temperature and fan speed control.
- Ensure your `remote_transmitter` and `remote_receiver` are correctly configured in ESPHome.
