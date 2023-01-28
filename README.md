# M5Stack CoreInk display component for ESPHome

M5Stack CoreInk display (gdew0154m09) external component for ESPHome

## Configuration

The easiest way to utilize the component is to use the [external components feature](https://esphome.io/components/external_components.html):

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/witasekl/gdew0154m09
      ref: master
```

The component uses the [SPI bus component](https://esphome.io/components/spi.html) and hence it requires the following pins to be defined:

* CLK / SCK: GPIO18 for M5Stack CoreInk
* MOSI / DIN: GPIO23 for M5Stack CoreInk

Besides from that the component itself requires the following pins:

* CS (chip select): GPIO9 for M5Stack CoreInk
* D/C (data / command): GPIO15 for M5Stack CoreInk
* BUSY: GPIO4 for M5Stack CoreInk
* RESET: GPIO0 for M5Stack CoreInk

Optionally you can also define the update interval (`update_interval`) for the component, but it's not recommended to use lower interval than 15s, because this could damage the display. The default value is 60s:

```yaml
display:
  - platform: gdew0154m09
    update_interval: 60s
```

## Example

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/witasekl/gdew0154m09
      ref: master

spi:
  clk_pin: GPIO18
  mosi_pin: GPIO23

display:
  - platform: gdew0154m09
    cs_pin: GPIO9
    dc_pin: GPIO15
    busy_pin: GPIO4
    reset_pin: GPIO0
    lambda: |-
      it.filled_circle(100, 100, 80);
```

## References

* <https://docs.m5stack.com/en/core/coreink>
* <https://m5stack.oss-cn-shenzhen.aliyuncs.com/resource/docs/datasheet/core/CoreInk-K048-GDEW0154M09%20V2.0%20Specification.pdf>
* <https://github.com/m5stack/M5Core-Ink>
