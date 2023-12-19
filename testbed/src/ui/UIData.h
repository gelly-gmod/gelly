#ifndef UIDATA_H
#define UIDATA_H

// Macros to help make the definition of data manipulated by UI clearer.
// Also to define windows

#define CREATE_UI_DATA(type, name) extern type name;

#define START_UI_DATA_GROUP(name) namespace UIData::name {
#define END_UI_DATA_GROUP() }

#define UI_DATA(group, name) UIData::group::name
#define DEFINE_UI_DATA(group, name, default) \
	decltype(default) UI_DATA(group, name) = default;

#define BIND_MEMBER_TO_UI_DATA(structName, group, name) \
	structName.name = UI_DATA(group, name);

#define DEFINE_WINDOW(name) void Render##name##Window();
#define IMPLEMENT_WINDOW(name) void Render##name##Window()
#define RENDER_WINDOW(name) Render##name##Window();

#endif	// UIDATA_H
