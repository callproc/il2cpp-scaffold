#pragma once

#include <Windows.h>
#include <string>

#include <common/include/console.h>

namespace sdk {
	namespace il2cpp {
		static HMODULE game_assembly = GetModuleHandleA("GameAssembly.dll");

		template<typename T>
		T func(const char* name) noexcept {
			const auto addr = GetProcAddress(game_assembly, name);
			if (addr == nullptr) {
				console::error("failed to resolve '{}'", name);
				return nullptr;
			}

			return (T)addr;
		}
	} // namespace il2cpp
} // namespace sdk

// types
typedef char16_t il2cpp_char;
typedef uintptr_t il2cpp_array_size;

struct il2cpp_domain_t {};
struct il2cpp_thread_t {};
struct il2cpp_assembly_t {};
struct il2cpp_image_t {};
struct il2cpp_class_t {};
struct il2cpp_type_t {};
struct il2cpp_field_t {};
struct il2cpp_method_t {};
struct il2cpp_monitor_t {};

struct il2cpp_method_info { void* ptr; };
struct il2cpp_object { il2cpp_class_t* klass; il2cpp_monitor_t* monitor; };

struct il2cpp_string : il2cpp_object {
	int32_t length;
	il2cpp_char chars[1];

	std::wstring to_wstring() {
		if (!this || length <= 0) return L"";
		return std::wstring(reinterpret_cast<wchar_t*>(chars), length);
	}

	std::string to_string() {
		if (!this || length <= 0) return "";

		std::wstring ws = to_wstring();
		int size = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), nullptr, 0, nullptr, nullptr);
		if (size <= 0) return "";

		std::string result(size, 0);
		WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), result.data(), size, nullptr, nullptr);

		return result;
	}
};

template<typename T = il2cpp_object*>
struct il2cpp_array_t : il2cpp_object
{
	void* bounds;
	int32_t max_length;
	T data[1];

	int32_t size() { return max_length; }
	T& operator[](int32_t i) { return data[i]; }
};
using il2cpp_array = il2cpp_array_t<>;

template<typename T = il2cpp_object*>
struct il2cpp_list_t : il2cpp_object
{
	il2cpp_array_t<T>* items;
	int32_t size;
	int32_t version;

	int32_t count() { return size; }
	T& operator[](int32_t i) { return items->data[i]; }
	T* begin() { return &items->data[0]; }
	T* end() { return &items->data[size]; }
};
using il2cpp_list = il2cpp_list_t<>;

template<typename K, typename V>
struct il2cpp_dictionary_t : il2cpp_object
{
	il2cpp_array_t<int32_t>* buckets;
	il2cpp_array_t<int32_t>* hash_codes;
	il2cpp_array_t<int32_t>* next;

	il2cpp_array_t<K>* keys;
	il2cpp_array_t<V>* values;

	int32_t                  count;
	int32_t                  free_list;
	int32_t                  free_count;
	int32_t                  version;

	int32_t size() { return count; }
};

// bindings
#define BIND(ret_type, name, args)			\
	typedef ret_type (*name##_t)args;		\
	static name##_t name = sdk::il2cpp::func<name##_t>(#name);

// _domain_
BIND(il2cpp_domain_t*, il2cpp_domain_get, ());
BIND(il2cpp_assembly_t*, il2cpp_domain_assembly_open, (il2cpp_domain_t* domain, const char* name));
BIND(il2cpp_assembly_t**, il2cpp_domain_get_assemblies, (il2cpp_domain_t* domain, size_t* size));

// _thread_
BIND(il2cpp_thread_t*, il2cpp_thread_attach, (il2cpp_domain_t* domain));
BIND(il2cpp_thread_t*, il2cpp_thread_current, ());
BIND(void, il2cpp_thread_detach, (il2cpp_thread_t* thread));
BIND(il2cpp_thread_t**, il2cpp_thread_get_all_attached_threads, (size_t* size));
BIND(void*, il2cpp_thread_get_frame_at, (int idx));
BIND(int, il2cpp_thread_get_stack_depth, ());
BIND(void*, il2cpp_thread_get_top_frame, ());
BIND(void, il2cpp_thread_walk_frame_stack, (void* func));

// _runtime_
BIND(void*, il2cpp_runtime_invoke, (void* method, void* object, void** params, void* exc));

// _assembly_
BIND(il2cpp_image_t*, il2cpp_assembly_get_image, (il2cpp_assembly_t* assembly));

// _image_
BIND(il2cpp_assembly_t*, il2cpp_image_get_assembly, (il2cpp_image_t* image));
BIND(il2cpp_class_t*, il2cpp_image_get_class, (il2cpp_image_t* image, size_t index));
BIND(size_t, il2cpp_image_get_class_count, (il2cpp_image_t* image));
BIND(il2cpp_method_t*, il2cpp_image_get_entry_point, (il2cpp_image_t* image));
BIND(const char*, il2cpp_image_get_filename, (il2cpp_image_t* image));
BIND(const char*, il2cpp_image_get_name, (il2cpp_image_t* image));

// _class_
BIND(int, il2cpp_class_array_element_size, (il2cpp_class_t* klass));
BIND(il2cpp_type_t*, il2cpp_class_enum_basetype, (il2cpp_class_t* klass));
BIND(void, il2cpp_class_for_each, (void (*func)(il2cpp_class_t*, void*), void* user_data));
BIND(il2cpp_class_t*, il2cpp_class_from_il2cpp_type, (il2cpp_type_t* type));
BIND(il2cpp_class_t*, il2cpp_class_from_name, (il2cpp_image_t* image, const char* namespace_name, const char* class_name));
BIND(il2cpp_class_t*, il2cpp_class_from_system_type, (il2cpp_object* type));
BIND(il2cpp_class_t*, il2cpp_class_from_type, (il2cpp_type_t* type));
BIND(const char*, il2cpp_class_get_assemblyname, (il2cpp_class_t* klass));
BIND(void, il2cpp_class_get_bitmap, (il2cpp_class_t* klass, size_t* bitmap));
BIND(size_t, il2cpp_class_get_bitmap_size, (il2cpp_class_t* klass));
BIND(int, il2cpp_class_get_data_size, (il2cpp_class_t* klass));
BIND(il2cpp_class_t*, il2cpp_class_get_declaring_type, (il2cpp_class_t* klass));
BIND(il2cpp_class_t*, il2cpp_class_get_element_class, (il2cpp_class_t* klass));
BIND(void*, il2cpp_class_get_events, (il2cpp_class_t* klass, void** iter));
BIND(il2cpp_field_t*, il2cpp_class_get_field_from_name, (il2cpp_class_t* klass, const char* name));
BIND(il2cpp_field_t*, il2cpp_class_get_fields, (il2cpp_class_t* klass, void** iter));
BIND(uint32_t, il2cpp_class_get_flags, (il2cpp_class_t* klass));
BIND(il2cpp_image_t*, il2cpp_class_get_image, (il2cpp_class_t* klass));
BIND(il2cpp_class_t*, il2cpp_class_get_interfaces, (il2cpp_class_t* klass, void** iter));
BIND(il2cpp_method_t*, il2cpp_class_get_method_from_name, (il2cpp_class_t* klass, const char* name, int param_count));
BIND(il2cpp_method_t*, il2cpp_class_get_methods, (il2cpp_class_t* klass, void** iter));
BIND(const char*, il2cpp_class_get_name, (il2cpp_class_t* klass));
BIND(const char*, il2cpp_class_get_namespace, (il2cpp_class_t* klass));
BIND(il2cpp_class_t*, il2cpp_class_get_nested_types, (il2cpp_class_t* klass, void** iter));
BIND(il2cpp_class_t*, il2cpp_class_get_parent, (il2cpp_class_t* klass));
BIND(void*, il2cpp_class_get_properties, (il2cpp_class_t* klass, void** iter));
BIND(void*, il2cpp_class_get_property_from_name, (il2cpp_class_t* klass, const char* name));
BIND(int, il2cpp_class_get_rank, (il2cpp_class_t* klass));
BIND(void*, il2cpp_class_get_static_field_data, (il2cpp_class_t* klass));
BIND(il2cpp_type_t*, il2cpp_class_get_type, (il2cpp_class_t* klass));
BIND(uint32_t, il2cpp_class_get_type_token, (il2cpp_class_t* klass));
BIND(int, il2cpp_class_get_userdata_offset, (il2cpp_class_t* klass));
BIND(bool, il2cpp_class_has_attribute, (il2cpp_class_t* klass, il2cpp_class_t* attr_class));
BIND(bool, il2cpp_class_has_parent, (il2cpp_class_t* klass, il2cpp_class_t* parent));
BIND(bool, il2cpp_class_has_references, (il2cpp_class_t* klass));
BIND(int, il2cpp_class_instance_size, (il2cpp_class_t* klass));
BIND(bool, il2cpp_class_is_abstract, (il2cpp_class_t* klass));
BIND(bool, il2cpp_class_is_assignable_from, (il2cpp_class_t* klass, il2cpp_class_t* oklass));
BIND(bool, il2cpp_class_is_blittable, (il2cpp_class_t* klass));
BIND(bool, il2cpp_class_is_enum, (il2cpp_class_t* klass));
BIND(bool, il2cpp_class_is_generic, (il2cpp_class_t* klass));
BIND(bool, il2cpp_class_is_inflated, (il2cpp_class_t* klass));
BIND(bool, il2cpp_class_is_inited, (il2cpp_class_t* klass));
BIND(bool, il2cpp_class_is_interface, (il2cpp_class_t* klass));
BIND(bool, il2cpp_class_is_subclass_of, (il2cpp_class_t* klass, il2cpp_class_t* klass2, bool check_interfaces));
BIND(bool, il2cpp_class_is_valuetype, (il2cpp_class_t* klass));
BIND(int, il2cpp_class_num_fields, (il2cpp_class_t* klass));
BIND(void, il2cpp_class_set_userdata, (il2cpp_class_t* klass, void* userdata));
BIND(int, il2cpp_class_value_size, (il2cpp_class_t* klass, uint32_t* align));

// _type_
BIND(bool, il2cpp_type_equals, (il2cpp_type_t* type, il2cpp_type_t* otherType));
BIND(const char*, il2cpp_type_get_assembly_qualified_name, (il2cpp_type_t* type));
BIND(uint32_t, il2cpp_type_get_attrs, (il2cpp_type_t* type));
BIND(il2cpp_class_t*, il2cpp_type_get_class_or_element_class, (il2cpp_type_t* type));
BIND(char*, il2cpp_type_get_name, (il2cpp_type_t* type));
BIND(void, il2cpp_type_get_name_chunked, (il2cpp_type_t* type, void (*chunk_func)(const char*, void*), void* user_data));
BIND(il2cpp_object*, il2cpp_type_get_object, (il2cpp_type_t* type));
BIND(const char*, il2cpp_type_get_reflection_name, (il2cpp_type_t* type));
BIND(int, il2cpp_type_get_type, (il2cpp_type_t* type));
BIND(bool, il2cpp_type_is_byref, (il2cpp_type_t* type));
BIND(bool, il2cpp_type_is_pointer_type, (il2cpp_type_t* type));
BIND(bool, il2cpp_type_is_static, (il2cpp_type_t* type));

// _field_
BIND(uint32_t, il2cpp_field_get_flags, (il2cpp_field_t* field));
BIND(const char*, il2cpp_field_get_name, (il2cpp_field_t* field));
BIND(int, il2cpp_field_get_offset, (il2cpp_field_t* field));
BIND(il2cpp_class_t*, il2cpp_field_get_parent, (il2cpp_field_t* field));
BIND(il2cpp_type_t*, il2cpp_field_get_type, (il2cpp_field_t* field));
BIND(void, il2cpp_field_get_value, (il2cpp_object* obj, il2cpp_field_t* field, void* value));
BIND(il2cpp_object*, il2cpp_field_get_value_object, (il2cpp_field_t* field, il2cpp_object* obj));
BIND(bool, il2cpp_field_has_attribute, (il2cpp_field_t* field, il2cpp_class_t* attr_class));
BIND(bool, il2cpp_field_is_literal, (il2cpp_field_t* field));
BIND(void, il2cpp_field_set_value, (il2cpp_object* obj, il2cpp_field_t* field, void* value));
BIND(void, il2cpp_field_set_value_object, (il2cpp_object* obj, il2cpp_field_t* field, il2cpp_object* value));
BIND(void, il2cpp_field_static_get_value, (il2cpp_field_t* field, void* value));
BIND(void, il2cpp_field_static_set_value, (il2cpp_field_t* field, void* value));

// _method_
BIND(il2cpp_class_t*, il2cpp_method_get_class, (il2cpp_method_t* method));
BIND(il2cpp_class_t*, il2cpp_method_get_declaring_type, (il2cpp_method_t* method));
BIND(uint32_t, il2cpp_method_get_flags, (il2cpp_method_t* method, uint32_t* iflags));
BIND(il2cpp_method_t*, il2cpp_method_get_from_reflection, (il2cpp_object* method));
BIND(const char*, il2cpp_method_get_name, (il2cpp_method_t* method));
BIND(il2cpp_object*, il2cpp_method_get_object, (il2cpp_method_t* method, il2cpp_class_t* refclass));
BIND(il2cpp_type_t*, il2cpp_method_get_param, (il2cpp_method_t* method, uint32_t index));
BIND(uint32_t, il2cpp_method_get_param_count, (il2cpp_method_t* method));
BIND(const char*, il2cpp_method_get_param_name, (il2cpp_method_t* method, uint32_t index));
BIND(il2cpp_type_t*, il2cpp_method_get_return_type, (il2cpp_method_t* method));
BIND(uint32_t, il2cpp_method_get_token, (il2cpp_method_t* method));
BIND(bool, il2cpp_method_has_attribute, (il2cpp_method_t* method, il2cpp_class_t* attr_class));
BIND(bool, il2cpp_method_is_generic, (il2cpp_method_t* method));
BIND(bool, il2cpp_method_is_inflated, (il2cpp_method_t* method));
BIND(bool, il2cpp_method_is_instance, (il2cpp_method_t* method));

// _string_
BIND(il2cpp_char*, il2cpp_string_chars, (il2cpp_string* str));
BIND(il2cpp_string*, il2cpp_string_intern, (il2cpp_string* str));
BIND(il2cpp_string*, il2cpp_string_is_interned, (il2cpp_string* str));
BIND(int32_t, il2cpp_string_length, (il2cpp_string* str));
BIND(il2cpp_string*, il2cpp_string_new, (const char* str));
BIND(il2cpp_string*, il2cpp_string_new_len, (const char* str, uint32_t length));
BIND(il2cpp_string*, il2cpp_string_new_utf16, (const il2cpp_char* text, int32_t len));
BIND(il2cpp_string*, il2cpp_string_new_wrapper, (const char* str));

// _object_
BIND(il2cpp_class_t*, il2cpp_object_get_class, (il2cpp_object* obj));
BIND(uint32_t, il2cpp_object_get_size, (il2cpp_object* obj));
BIND(il2cpp_method_t*, il2cpp_object_get_virtual_method, (il2cpp_object* obj, il2cpp_method_t* method));
BIND(uint32_t, il2cpp_object_header_size, ());
BIND(il2cpp_object*, il2cpp_object_new, (il2cpp_class_t* klass));
BIND(void*, il2cpp_object_unbox, (il2cpp_object* obj));

// _array_
BIND(il2cpp_class_t*, il2cpp_array_class_get, (il2cpp_class_t* element_class, uint32_t rank));
BIND(int, il2cpp_array_element_size, (il2cpp_class_t* array_class));
BIND(uint32_t, il2cpp_array_get_byte_length, (il2cpp_array* array));
BIND(il2cpp_array_size, il2cpp_array_length, (il2cpp_array* array));
BIND(il2cpp_array*, il2cpp_array_new, (il2cpp_class_t* elementTypeInfo, il2cpp_array_size length));
BIND(il2cpp_array*, il2cpp_array_new_full, (il2cpp_class_t* array_class, il2cpp_array_size* lengths, il2cpp_array_size* lower_bounds));
BIND(il2cpp_array*, il2cpp_array_new_specific, (il2cpp_class_t* arrayTypeInfo, il2cpp_array_size length));
BIND(uint32_t, il2cpp_array_object_header_size, ());