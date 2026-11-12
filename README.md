# ERDI laser rangefinder for ESPHome and Home Assistant

Datasheets and public prices are on the [LRF22VB page](https://erdilrf.com/models/lrf22vb/?utm_source=github&utm_medium=readme&utm_campaign=erdi-lrf-home-assistant) and in the [full ERDI laser rangefinder model list](https://erdilrf.com/models/?utm_source=github&utm_medium=readme&utm_campaign=erdi-lrf-home-assistant).

An **ESPHome external component** that reads an **ERDI 905 nm laser rangefinder module** over UART and exposes the distance as a **Home Assistant distance sensor**. Use it for a **laser distance sensor in Home Assistant**: water or fuel tank level, garage parking assist, door and gate position, snow depth, object presence at metre-scale range. Works on **ESP32 / ESP8266** boards with a free UART. Covers the **LRF22VB, LRF50VB, LRF50H-LRF1500H** `5C` stream modules (460800 baud) and the **LR1000E2 ... LR2000E4, LRF1200A1, LRF3000A1, SPD1200N\*** polled modules (115200 baud).

## Quick start

```yaml
external_components:
  - source: github://ErdiTech/erdi-lrf-home-assistant
    components: [erdi_lrf]

uart:
  id: lrf_uart
  tx_pin: GPIO17
  rx_pin: GPIO16
  baud_rate: 460800          # 115200 for LR / LRF A / SPD models

sensor:
  - platform: erdi_lrf
    uart_id: lrf_uart
    model: LRF22VB
    name: "Laser distance"
    update_interval: 1s
```

Add the node in the ESPHome dashboard; Home Assistant discovers it and shows `Laser distance` in metres with the `distance` device class.

## Options

| Option | Values | Notes |
|---|---|---|
| `model` | `LRF22VB`, `LRF50VB`, `LRF50H`, `LRF100H`, `LRF200H`, `LRF300H`, `LRF600H`, `LRF1500H`, `LR1000E2`, `LR1000E4`, `LR1200E2`, `LR1200E4`, `LR1500E2`, `LR1500E4`, `LR2000E2`, `LR2000E4`, `LRF1200A1`, `LRF3000A1`, `SPD1200N0`, `SPD1200N2`, `SPD1200N4` | Selects frame format, unit and out-of-range marker. The unit differs by series (H: cm, VB: mm, family A: metres x 10), so the model is required. |
| `update_interval` | time | How often the state is published. Family A modules are polled once per interval. Default `1s`. |
| `uart_id` | id | UART whose baud rate matches the model; a warning is logged if it does not. |

Behaviour: out-of-range frames and polled "no echo" replies publish `unknown`; after three update intervals without a valid frame the sensor also reports `unknown` instead of a stale value. Use ESPHome `filters:` (median, sliding window) as usual.

## Wiring

| Module | ESP32 |
|---|---|
| TX | RX pin of the UART (GPIO16 above) |
| RX | TX pin (GPIO17) |
| GND | GND |
| VCC | per the model manual, from its own regulator |

Check the module's TX logic level before connecting to a 3.3 V GPIO. Pin order and levels are model-specific: use the model page at <https://erdilrf.com/models/?utm_source=github&utm_medium=readme&utm_campaign=erdi-lrf-home-assistant>. 460800 baud needs a hardware UART (not the software UART on ESP8266).

## Frame formats

| Family | Frame | Check |
|---|---|---|
| B (stream, 460800) | `5C lo hi chk`; LRF1500H `5C lo mid hi chk` | `~(sum of distance bytes)` |
| A (polled, 115200) | request `55 AA 88 FF FF FF FF 84`, reply `55 AA 88 STA FF DH DL SUM` | reply sum includes header |

The parser is a dependency-free C99 header ([`erdi_lrf_core.h`](components/erdi_lrf/erdi_lrf_core.h)) tested on the host:

```bash
gcc -std=c99 -Wall -Wextra -o t test/test_core.c -lm && ./t
esphome config example.yaml
esphome compile example.yaml
```

Protocol source: [ErdiTech/erdi-lrf-drivers](https://github.com/ErdiTech/erdi-lrf-drivers).

## Related projects

- [erdi-lrf-esp32-micropython](https://github.com/ErdiTech/erdi-lrf-esp32-micropython): the same parser for MicroPython
- [erdi-lrf-stm32-hal](https://github.com/ErdiTech/erdi-lrf-stm32-hal): STM32 HAL driver
- [laser-rangefinder-module-database](https://github.com/ErdiTech/laser-rangefinder-module-database): dataset of laser rangefinder modules

## Support / 打赏

If this project saved you time, you can buy the author a coffee in crypto. Thank you!
如果这个项目帮到了你，欢迎用加密货币打赏支持，谢谢！

| Network | Asset | Address |
|---|---|---|
| TRON (TRC20) | USDT | `TNGdTwE6opH6rmUyoG1JUR7FSyEb4k273R` |
| Ethereum / BNB Chain / Polygon | USDT | `0x59c43144a862b351fa1086ae842d1327c9d91613` |
| Ethereum / Arbitrum / Base | ETH | `0x59c43144a862b351fa1086ae842d1327c9d91613` |
| Bitcoin | BTC | `1RS6tWXHregLfEsiPzVUCUBuhgDg5uTSV` |
| Solana | USDT | `HZguEXfTtLChNdJTxAYCbcGscaqikz8JYrEpUhEyeVd9` |

Please send only the listed asset on the listed network — other tokens or networks may be lost.
请按表中对应的网络转对应的币种，转错币种或网络可能无法找回。

## License

MIT — see [LICENSE](LICENSE).
