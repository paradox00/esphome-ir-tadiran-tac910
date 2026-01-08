import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_NBITS

from esphome.components import remote_base

CONF_CURRENT_TEMP = "current_temp"
CONF_SET_TEMP = "set_temp"
CONF_SPEED = "speed"
CONF_STATE = "state"

#Tac protocol
TacData, TacBinarySensor, TacTrigger, TacAction, TacDumper = remote_base.declare_protocol("Tac")

TAC_SCHEME = cv.Schema(
    {
        cv.Required(CONF_NBITS): cv.int_range(min=15, max=56),
        cv.Optional(CONF_CURRENT_TEMP): cv.uint8_t,
        cv.Optional(CONF_SET_TEMP): cv.uint8_t,
        cv.Optional(CONF_SPEED): cv.uint8_t,
        cv.Optional(CONF_STATE): cv.uint8_t,
    }
)

@remote_base.register_dumper("Tac", TacDumper)
def tac_dumper(var, config):
        pass

@remote_base.register_trigger("Tac", TacTrigger, TacData)
def tac_trigger(var, config):
        pass

@remote_base.register_action("Tac", TacAction, TAC_SCHEME)
async def tac_action(var, config, args):
    cg.add(var.set_nbits(await cg.templatable(config[CONF_NBITS], args, cg.uint8)))
    cg.add(var.set_current_temp(await cg.templatable(config[CONF_CURRENT_TEMP], args, cg.uint8)))
    cg.add(var.set_desired_temp(await cg.templatable(config[CONF_SET_TEMP], args, cg.uint8)))
    cg.add(var.set_speed(await cg.templatable(config[CONF_SPEED], args, cg.uint8)))
    cg.add(var.set_state(await cg.templatable(config[CONF_STATE], args, cg.uint8)))

# #################################################################################
# # component
# ir_tadiran_tac910_ns = cg.esphome_ns.namespace("ir_tadiran_tac910")
# IrTadiranTac910 = ir_tadiran_tac910_ns.class_("IrTadiranTac910", cg.Component)

# CONFIG_SCHEMA = cv.Schema(
#     {
#         cv.GenerateID(): cv.declare_id(IrTadiranTac910),
#     }
# ).extend(cv.COMPONENT_SCHEMA)

# async def to_code(config):
#     var = cg.new_Pvariable(config[CONF_ID])
#     await cg.register_component(var, config)
