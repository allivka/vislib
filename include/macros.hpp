#pragma once


#define DECL_SETTER(field) \
    void set_ ## field(const typeof(field)& param) {this->field = param;}

#define DECL_GETTER(field) \
    typeof(field) get_ ## field() const {return this->field;}

#define DECL_SETTER_VIRTUAL(field) \
    virtual void set_ ## field(const typeof(field)& param) {this->field = param;}

#define DECL_GETTER_VIRTUAL(field) \
    virtual typeof(field) get_ ## field() const {return this->field;}




#define FIELD_WITH_SETTER(type, field) \
    type field; void set_ ## field(const typeof(field)& param) {this->field = param;}

#define FIELD_WITH_GETTER(field) \
    type; typeof(field) get_ ## field() const {return this->field;}

#define FIELD_WITH_SETTER_VIRTUAL(field) \
    type; virtual void set_ ## field(const typeof(field)& param) {this->field = param;}

#define FIELD_WITH_GETTER_VIRTUAL(field) \
    type; virtual typeof(field) get_ ## field() const {return this->field;}





#define FIELD_WITH_SETTER_NT(field) \
    field; void set_ ## field(const typeof(field)& param) {this->field = param;}

#define FIELD_WITH_GETTER_NT(field) \
    field; typeof(field) get_ ## field() const {return this->field;}

#define FIELD_WITH_SETTER_VIRTUAL_NT(field) \
    field; virtual void set_ ## field(const typeof(field)& param) {this->field = param;}

#define FIELD_WITH_GETTER_VIRTUAL_NT(field) \
    field; virtual typeof(field) get_ ## field() const {return this->field;}





#define FABRICATE_FIELD(class_name, field) \
    class_name& set_ ## field(const typeof(field)& param) {this->field = param; return *this;} \
    DECL_GETTER(field) \

#define FIELD_WITH_FABRIC(class_name, type, field) \
    type field; class_name& set_ ## field(const typeof(field)& param) {this->field = param; return *this;} \
    DECL_GETTER(field)

#define FIELD_WITH_FABRIC_NT(class_name, field) \
    field; class_name& set_ ## field(const typeof(field)& param) {this->field = param; return *this;} \
    DECL_GETTER(field)

#define FABRICATE_FIELD_VIRTUAL(class_name, field) \
    virtual class_name& set_ ## field(const typeof(field)& param) {this->field = param; return *this;} \
    DECL_GETTER_VIRTUAL(field) \

#define FIELD_WITH_FABRIC_VIRTUAL(class_name, type, field) \
    type field; virtual class_name& set_ ## field(const typeof(field)& param) {this->field = param; return *this;} \
    DECL_GETTER_VIRTUAL(field)

#define FIELD_WITH_FABRIC_VIRTUAL_NT(class_name, field) \
    field; virtual class_name& set_ ## field(const typeof(field)& param) {this->field = param; return *this;} \
    DECL_GETTER_VIRTUAL(field)
