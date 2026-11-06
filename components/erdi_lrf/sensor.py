import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, uart
from esphome.const import (
    DEVICE_CLASS_DISTANCE,
    STATE_CLASS_MEASUREMENT,
    UNIT_METER,
)

DEPENDENCIES = ["uart"]
CODEOWNERS = ["@Tommyapi"]

erdi_lrf_ns = cg.esphome_ns.namespace("erdi_lrf")
ErdiLRFSensor = erdi_lrf_ns.class_(
    "ErdiLRFSensor", sensor.Sensor, cg.PollingComponent, uart.UARTDevice
)

CONF_MODEL = "model"

# Models with a published UART protocol (family A "55 AA" polled, family B "5C" stream).
MODELS = [
    "LR1000E2", "LR1000E4", "LR1200E2", "LR1200E4", "LR1500E2", "LR1500E4",
    "LR2000E2", "LR2000E4", "LRF1200A1", "LRF3000A1", "SPD1200N0", "SPD1200N2",
    "SPD1200N4", "LRF50H", "LRF100H", "LRF200H", "LRF300H", "LRF600H",
    "LRF1500H", "LRF22VB", "LRF50VB",
]

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        ErdiLRFSensor,
        unit_of_measurement=UNIT_METER,
        accuracy_decimals=3,
        device_class=DEVICE_CLASS_DISTANCE,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend({cv.Required(CONF_MODEL): cv.one_of(*MODELS, upper=True)})
    .extend(cv.polling_component_schema("1s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    cg.add(var.set_model(config[CONF_MODEL]))
