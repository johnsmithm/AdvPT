static string defaultTechTree = "#name,minerals,vespene,build time,supply cost,supply provided,start energy,max energy,race,produced_by,dependency,built_type,is_building \n\
###########,,,,,,,,,, \n\
# Protoss #,,,,,,,,,, \n\
###########,,,,,,,,,, \n\
probe,50,0,17,1,0,0,0,protoss, nexus , ,active, 0 \n\
zealot,100,0,38,2,0,0,0,protoss, gateway , ,active, 0 \n\
stalker,125,50,42,2,0,0,0,protoss, gateway ,cybernetics_core ,active, 0 \n\
sentry,50,100,37,2,0,0,0,protoss, gateway ,cybernetics_core ,active, 0 \n\
warp_prism,200,0,50,2,0,0,0,protoss, robotics_facility , ,active, 0 \n\
immortal,250,100,55,3,0,0,0,protoss, robotics_facility , ,active, 0 \n\
observer,25,75,30,1,0,0,0,protoss, robotics_facility , ,active, 0 \n\
colossus,300,200,75,6,0,0,0,protoss, robotics_facility ,robotics_bay ,active, 0 \n\
high_templar,50,150,55,2,0,0,0,protoss, gateway ,templar_archives ,active, 0 \n\
dark_templar,125,125,55,2,0,0,0,protoss, gateway ,dark_shrine ,active, 0 \n\
mothership,400,400,160,8,0,0,0,protoss, nexus ,fleet_beacon ,active, 0 \n\
phoenix,150,100,35,2,0,0,0,protoss, stargate , ,active, 0 \n\
void_ray,250,150,60,3,0,0,0,protoss, stargate , ,active, 0 \n\
carrier,350,250,120,6,0,0,0,protoss, stargate ,fleet_beacon ,active, 0 \n\
#buildings,,,,,0,,,,, \n\
nexus,400,0,100,0,10,0,100,protoss, probe , ,warp, 1\n\
pylon,100,0,25,0,8,0,0,protoss, probe , ,warp, 1\n\
gateway,150,0,65,0,0,0,0,protoss, probe ,pylon ,warp, 1\n\
cybernetics_core,150,0,50,0,0,0,0,protoss, probe ,gateway ,warp, 1\n\
robotics_facility,200,100,65,0,0,0,0,protoss, probe ,cybernetics_core ,warp, 1\n\
robotics_bay,200,200,65,0,0,0,0,protoss, probe ,robotics_facility ,warp, 1\n\
twilight_council,150,100,50,0,0,0,0,protoss, probe ,cybernetics_core ,warp, 1\n\
templar_archives,150,200,50,0,0,0,0,protoss, probe ,twilight_council ,warp, 1\n\
dark_shrine,100,150,100,0,0,0,0,protoss, probe ,twilight_council ,warp, 1\n\
stargate,150,150,60,0,0,0,0,protoss, probe ,cybernetics_core ,warp, 1\n\
fleet_beacon,300,200,60,0,0,0,0,protoss, probe ,stargate ,warp, 1\n\
assimilator,75,0,30,0,0,0,0,protoss, probe , ,warp, 1\n\
forge,150,0,45,0,0,0,0,protoss, probe ,nexus ,warp, 1\n\
photon_cannon,150,0,40,0,0,0,0,protoss, probe ,forge ,warp, 1\n\
#,,,,,,,,,, ,\n\
##########,,,,,,,,,, \n\
#  Zerg  #,,,,,,,,,, \n\
##########,,,,,,,,,, \n\
overlord,100,0,25,0,8,0,0,zerg, larva , ,morph, 0\n\
overseer,50,50,17,0,8,0,0,zerg, overlord ,lair/hive ,morph, 0\n\
hydralisk,100,50,33,2,0,0,0,zerg, larva ,hydralisk_den ,morph, 0\n\
drone,50,0,17,1,0,0,0,zerg, larva , ,morph, 0\n\
queen,150,0,50,2,0,25,200,zerg, hatchery/lair/hive,spawning_pool ,active, 0\n\
larva,0,0,0,0,0,0,0,zerg, hatchery/lair/hive, ,morph, 0\n\
zergling,50,0,24,0.5,0,0,0,zerg, larva ,spawning_pool ,morph, 0\n\
baneling,25,25,20,0.5,0,0,0,zerg, zergling ,baneling_nest ,morph, 0\n\
roach,75,25,27,2,0,0,0,zerg, larva ,roach_warren ,morph, 0\n\
infestor,100,150,50,2,0,0,0,zerg, larva ,infestation_pit ,morph, 0\n\
nydus_worm,100,100,20,0,0,0,0,zerg, nydus_network , ,active, 0\n\
ultralisk,300,200,55,6,0,0,0,zerg, larva ,ultralisk_cavern ,morph, 0\n\
brood_lord,150,150,34,4,0,0,0,zerg, corruptor ,greater_spire ,morph, 0\n\
corruptor,150,100,40,2,0,0,0,zerg, larva ,spire/greater_spire ,morph, 0\n\
mutalisk,100,100,33,2,0,0,0,zerg, larva ,spire/greater_spire ,morph, 0\n\
#buildings,,,,,,,,,, \n\
hatchery,300,0,100,0,2,0,0,zerg,drone , ,morph, 1\n\
evolution_chamber,75,0,35,0,0,0,0,zerg,drone ,hatchery ,morph, 1\n\
spore_crawler,75,0,30,0,0,0,0,zerg,drone ,evolution_chamber ,morph, 1\n\
lair,150,100,80,0,2,0,0,zerg,hatchery ,hatchery/spawning_pool ,morph, 1\n\
spawning_pool,200,0,65,0,0,0,0,zerg,drone ,hatchery ,morph, 1\n\
extractor,25,0,30,0,0,0,0,zerg,drone , ,morph, 1\n\
spine_crawler,100,0,50,0,0,0,0,zerg,drone ,spawning_pool ,morph, 1\n\
roach_warren,150,0,55,0,0,0,0,zerg,drone ,spawning_pool ,morph, 1\n\
baneling_nest,100,50,60,0,0,0,0,zerg,drone ,spawning_pool ,morph, 1\n\
hydralisk_den,100,100,40,0,0,0,0,zerg,drone ,lair ,morph, 1\n\
infestation_pit,100,100,50,0,0,0,0,zerg,drone ,lair ,morph, 1\n\
hive,200,150,100,0,2,0,0,zerg,lair ,infestation_pit ,morph, 1\n\
nydus_network,150,200,50,0,0,0,0,zerg,drone ,lair ,morph, 1\n\
ultralisk_cavern,150,200,65,0,0,0,0,zerg,drone ,hive ,morph, 1\n\
greater_spire,100,150,100,0,0,0,0,zerg,spire ,hive ,morph, 1\n\
spire,200,200,100,0,0,0,0,zerg,drone ,lair ,morph, 1\n\
#,,,,,,,,,, \n\
##########,,,,,,,,,, \n\
# Terran #,,,,,,,,,, \n\
##########,,,,,,,,,, \n\
scv,50,0,17,1,0,0,0,terran,command_center/orbital_command/planetary_fortress, ,active , 0\n\
marine,50,0,25,1,0,0,0,terran, barracks/barracks_with_tech_lab/barracks_with_reactor, ,active , 0\n\
marauder,100,25,30,2,0,0,0,terran, barracks_with_tech_lab , ,active , 0\n\
reaper,50,50,45,1,0,0,0,terran, barracks_with_tech_lab , ,active , 0\n\
ghost,200,100,40,2,0,0,0,terran, barracks_with_tech_lab ,ghost_academy ,active , 0\n\
hellion,100,0,30,2,0,0,0,terran,factory/factory_with_reactor/factory_with_tech_lab, ,active , 0\n\
siege_tank,150,125,45,3,0,0,0,terran, factory_with_tech_lab , ,active , 0\n\
thor,300,200,60,6,0,0,0,terran, factory_with_tech_lab ,armory ,active , 0\n\
medivac,100,100,42,2,0,0,0,terran,starport/starport_with_reactor/starport_with_tech_lab, ,active , 0\n\
viking,150,75,42,2,0,0,0,terran,starport/starport_with_reactor/starport_with_tech_lab, ,active , 0\n\
raven,100,200,60,2,0,0,0,terran,starport_with_tech_lab , ,active , 0\n\
banshee,150,100,60,3,0,0,0,terran,starport_with_tech_lab , ,active , 0\n\
battlecruiser,400,300,90,6,0,0,0,terran,starport_with_tech_lab ,fusion_core ,active , 0\n\
#buildings,,,,,0,,,,, \n\
command_center,400,0,100,0,11,0,0,terran,scv , ,active, 1\n\
orbital_command,150,0,35,0,11,50,200,terran,command_center,barracks/barracks_with_tech_lab/barracks_with_reactor ,morph, 1\n\
planetary_fortress,150,150,50,0,11,0,0,terran,orbital_command,engineering_bay ,morph, 1\n\
refinery,75,0,30,0,0,0,0,terran,scv , ,active, 1\n\
engineering_bay,125,0,35,0,0,0,0,terran,scv , ,active, 1\n\
missile_turret,100,0,25,0,0,0,0,terran,scv ,engineering_bay ,active, 1\n\
sensor_tower,125,100,25,0,0,0,0,terran,scv ,engineering_bay ,active, 1\n\
barracks,150,0,65,0,0,0,0,terran,scv ,supply_depot ,active, 1\n\
factory,150,100,60,0,0,0,0,terran,scv ,barracks/barracks_with_tech_lab/barracks_with_reactor ,active, 1\n\
armory,150,100,65,0,0,0,0,terran,scv ,barracks/barracks_with_tech_lab/barracks_with_reactor ,active, 1\n\
bunker,100,0,35,0,0,0,0,terran,scv ,barracks/barracks_with_tech_lab/barracks_with_reactor ,active, 1\n\
ghost_academy,150,50,40,0,0,0,0,terran,scv ,barracks/barracks_with_tech_lab/barracks_with_reactor ,active, 1\n\
starport,150,100,50,0,0,0,0,terran,scv ,barracks/barracks_with_tech_lab/barracks_with_reactor ,active, 1\n\
fusion_core,150,150,65,0,0,0,0,terran,scv ,barracks/barracks_with_tech_lab/barracks_with_reactor ,active, 1\n\
supply_depot,100,0,30,0,8,0,0,terran,scv , ,active, 1\n\
barracks_with_reactor,50,50,50,0,0,0,0,terran,barracks , ,morph, 1\n\
barracks_with_tech_lab,50,25,25,0,0,0,0,terran,barracks , ,morph, 1\n\
starport_with_reactor,50,50,50,0,0,0,0,terran,starport , ,morph, 1\n\
starport_with_tech_lab,50,25,25,0,0,0,0,terran,starport , ,morph, 1\n\
factory_with_reactor,50,50,50,0,0,0,0,terran,factory , ,morph, 1\n\
factory_with_tech_lab,50,25,25,0,0,0,0,terran,factory , ,morph, 1 \
";
