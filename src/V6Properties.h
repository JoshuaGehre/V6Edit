#ifndef V6PROPERTIES_H_INCLUDED
#define V6PROPERTIES_H_INCLUDED

// Size of a room in blocks
#define V6_ROOM_X 40

#define V6_ROOM_Y 30

// Maximum level size in rooms
#define V6_ROOMS_X 20

#define V6_ROOMS_Y 20

// Maximum number of rooms in a level
#define V6_ROOM_XY (V6_ROOM_X * V6_ROOM_Y)

// Ved stores its data in an
// entity at this location
#define VED_ENTITY_X (V6_ROOM_X * V6_ROOMS_X)

#define VED_ENTITY_Y (V6_ROOM_Y * V6_ROOMS_Y)

#define VED_ENTITY_X_ALT 4000

#define VED_ENTITY_Y_ALT 3000

#endif // V6PROPERTIES_H_INCLUDED
