//
// Created by liu on 18-10-23.
//
// Modified by yuan on 20-10-24:
// [For DuplicateQuery]
// 1. add a copyValue interface to iterator type
// [For EraseQuery]
// 2. add an erase interface to the Table type

#ifndef PROJECT_DB_TABLE_H
#define PROJECT_DB_TABLE_H

#include "../utils/uexception.h"
#include "../utils/formatter.h"
//#include "../query/Query.h"

#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <list>
#include <mutex>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>

class Query;


#define _DBTABLE_ACCESS_WITH_NAME_EXCEPTION(field)\
do {\
    try {\
        auto &index = table->fieldMap.at(field);\
        return it->datum.at(index);\
    } catch (const std::out_of_range& e) {\
        throw TableFieldNotFound (\
            R"(Field name "?" doesn't exists.)"_f % (field)\
        );\
    }\
} while(0)

#define _DBTABLE_ACCESS_WITH_INDEX_EXCEPTION(index)\
do {\
    try {\
        return it->datum.at(index);\
    } catch (const std::out_of_range& e) {\
        throw TableFieldNotFound (\
            R"(Field index ? out of range.)"_f % (index)\
        );\
    }\
} while(0)

class Table {
    friend class DuplicateQuery;
    friend class DuplicateQueryTaskResult;
public:
    typedef std::string KeyType;
    typedef std::string FieldNameType;
    typedef size_t FieldIndex;
    typedef int ValueType;
    static constexpr const ValueType ValueTypeMax = std::numeric_limits<ValueType>::max();
    static constexpr const ValueType ValueTypeMin = std::numeric_limits<ValueType>::min();
    typedef size_t SizeType;

private:
    /** A row in the table */
    std::list<Query *> queryQueue;
    std::mutex queueMutex;
    int ReadwriteSem=0;
    // use queueMutex
    bool isReady = false;
    struct Datum {
        /** Unique key of this datum */
        KeyType key;
        /** The values in the order of fields */
        std::vector<ValueType> datum;

        Datum() = default;

        Datum(const Datum &) = default;

        explicit Datum(const SizeType &size) {
            datum = std::vector<ValueType>(size, ValueType());
        }

        template<class ValueTypeContainer>
        explicit Datum(const KeyType &key, const ValueTypeContainer &datum) {
            this->key = key;
            this->datum = datum;
        }

        explicit Datum(const KeyType &key, std::vector<ValueType> &&datum) noexcept {
            this->key = key;
            this->datum = std::move(datum);
        }
    };

    typedef std::vector<Datum>::iterator DataIterator;
    typedef std::vector<Datum>::const_iterator ConstDataIterator;

    /** The fields, ordered as defined in fieldMap */
    std::vector<FieldNameType> fields;
    /** Map field name into index */
    std::unordered_map<FieldNameType, FieldIndex> fieldMap;

    /** The rows are saved in a vector, which is unsorted */
    std::vector<Datum> data;
    /** Used to keep the keys unique and provide O(1) access with key */
    std::unordered_map<KeyType, SizeType> keyMap;

    /** The name of table */
    std::string tableName;

public:
    typedef std::unique_ptr<Table> Ptr;

    /**
     * A proxy class that provides abstraction on internal Implementation.
     * Allows independent variation on the representation for a table object
     *
     * @tparam Iterator
     * @tparam VType
     */
    template<class Iterator, class VType>
    class ObjectImpl {
        friend class Table;

        /** Not const because key can be updated */
        Iterator it;
        Table *table;

    public:
        typedef std::unique_ptr<ObjectImpl> Ptr;

        ObjectImpl(Iterator datumIt, const Table *t)
                : it(datumIt), table(const_cast<Table *>(t)) {}

        ObjectImpl(const ObjectImpl &) = default;

        ObjectImpl(ObjectImpl &&) noexcept = default;

        ObjectImpl &operator=(const ObjectImpl &) = default;

        ObjectImpl &operator=(ObjectImpl &&) noexcept = default;

        ~ObjectImpl() = default;

        KeyType key() const { return it->key; }

        // [For DuplicateQuery], return a copy of value.
        std::vector<ValueType> copyValue() const { return it->datum; }

        void setKey(KeyType key) {
            auto keyMapIt = table->keyMap.find(it->key);
            auto dataIt = std::move(keyMapIt->second);
            table->keyMap.erase(keyMapIt);
            table->keyMap.emplace(key, std::move(dataIt));
            it->key = std::move(key);
        }

        /**
         * Accessing by index should be, at least as fast as accessing by field name.
         * Clients should prefer accessing by index if the same field is accessed frequently
         * (the implement is improved so that index is actually faster now)
         */
        VType &operator[](const FieldNameType &field) const {
            _DBTABLE_ACCESS_WITH_NAME_EXCEPTION(field);
        }

        VType &operator[](const FieldIndex &index) const {
            _DBTABLE_ACCESS_WITH_INDEX_EXCEPTION(index);
        }

        VType &get(const FieldNameType &field) const {
            _DBTABLE_ACCESS_WITH_NAME_EXCEPTION(field);
        }

        VType &get(const FieldIndex &index) const {
            _DBTABLE_ACCESS_WITH_INDEX_EXCEPTION(index);
        }
    };


    typedef ObjectImpl<DataIterator, ValueType> Object;
    typedef ObjectImpl<ConstDataIterator, const ValueType> ConstObject;

    /**
     * A proxy class that provides iteration on the table
     * @tparam ObjType
     * @tparam DatumIterator
     */
    template<typename ObjType, typename DatumIterator>
    class IteratorImpl {
        using difference_type   = std::ptrdiff_t;
        using value_type        = ObjType;
        using pointer           = typename ObjType::Ptr;
        using reference         = ObjType;
        using iterator_category = std::random_access_iterator_tag;
        // See https://stackoverflow.com/questions/37031805/

        friend class Table;

        DatumIterator it;
        const Table *table = nullptr;

    public:
        IteratorImpl(DatumIterator datumIt, const Table *t)
                : it(datumIt), table(t) {}

        IteratorImpl() = default;

        IteratorImpl(const IteratorImpl &) = default;

        IteratorImpl(IteratorImpl &&) noexcept = default;

        IteratorImpl &operator=(const IteratorImpl &) = default;

        IteratorImpl &operator=(IteratorImpl &&) noexcept = default;

        ~IteratorImpl() = default;

        pointer operator->() { return createProxy(it, table); }

        reference operator*() { return *createProxy(it, table); }

        IteratorImpl operator+(int n) { return IteratorImpl(it + n, table); }

        IteratorImpl operator-(int n) { return IteratorImpl(it - n, table); }

        IteratorImpl &operator+=(int n) { return it += n, *this; }

        IteratorImpl &operator-=(int n) { return it -= n, *this; }

        IteratorImpl &operator++() { return ++it, *this; }

        IteratorImpl &operator--() { return --it, *this; }

        const IteratorImpl operator++(int) {
            auto retVal = IteratorImpl(*this);
            ++it;
            return retVal;
        }

        const IteratorImpl operator--(int) {
            auto retVal = IteratorImpl(*this);
            --it;
            return retVal;
        }

        bool operator==(const IteratorImpl &other) { return this->it == other.it; }

        bool operator!=(const IteratorImpl &other) { return this->it != other.it; }

        bool operator<=(const IteratorImpl &other) { return this->it <= other.it; }

        bool operator>=(const IteratorImpl &other) { return this->it >= other.it; }

        bool operator<(const IteratorImpl &other) { return this->it < other.it; }

        bool operator>(const IteratorImpl &other) { return this->it > other.it; }
    };

    typedef IteratorImpl<Object, decltype(data.begin())> Iterator;
    typedef IteratorImpl<ConstObject, decltype(data.cbegin())> ConstIterator;

private:
    static ConstObject::Ptr createProxy(ConstDataIterator it, const Table *table) {
        return std::make_unique<ConstObject>(it, table);
    }

    static Object::Ptr createProxy(DataIterator it, const Table *table) {
        return std::make_unique<Object>(it, table);
    }

public:
    Table() = delete;

    explicit Table(std::string name) : tableName(std::move(name)) {}

    void addQueryToTable(Query* query);
    
    void notifyQueryComplete();

    void makeReady();

    /**
     * Accept any container that contains string.
     * @tparam FieldIDContainer
     * @param name: the table name (must be unique in the database)
     * @param fields: an iterable container with fields
     */
    template<class FieldIDContainer>
    Table(const std::string &name, const FieldIDContainer &fields);


    template<class FieldIDContainer>
    void updateFields(const FieldIDContainer &fields) {
        this->fields.insert(this->fields.end(), fields.cbegin(), fields.cend());
        SizeType i = 0;
        for (const auto &field : fields) {
            if (field == "KEY")
                throw MultipleKey(
                        "Error creating table \"" + name() + "\": Multiple KEY field."
                );
            fieldMap.emplace(field, i++);
        }
    }

    void copyDataFrom(const Table& src) {
        data = decltype(data)(src.data);
        fields = decltype(fields)(src.fields);
        fieldMap = decltype(fieldMap)(src.fieldMap);
        keyMap = decltype(keyMap)(src.keyMap);
    }

    // for drop use, keep the table object in database
    void clearAllData() {
        data.clear();
        fields.clear();
        fieldMap.clear();
        keyMap.clear();
    }

    void fillTableWithFile(const std::string &filename) {
        Table::SizeType fieldCount;
        std::deque<Table::KeyType> fields;

        std::ifstream infile(filename);
        std::string line;
        std::stringstream sstream;
        if (!std::getline(infile, line)) {
            throw std::runtime_error("Table::fillTableWithFile: GETLINE ERROR!");
        }

        sstream.str(line);
        std::string tb_name;
        sstream >> tb_name >> fieldCount;
        if (!sstream) {
            throw std::runtime_error("Table::fillTableWithFile: Failed to parse table metadata!");
        }

        if (!(std::getline(infile, line))) {
            throw std::runtime_error("Table::fillTableWithFile: Failed to load field names!");
        }

        sstream.clear();
        sstream.str(line);
        for (Table::SizeType i = 0; i < fieldCount; ++i) {
            std::string field;
            if (!(sstream >> field)) {
                throw std::runtime_error("Table::fillTableWithFile: Failed to load field names!");
            } else {
                fields.emplace_back(std::move(field));
            }
        }

        if (fields.front() != "KEY") {
            throw std::runtime_error("Table::fillTableWithFile: Missing or invalid KEY field!");
        }

        fields.erase(fields.begin()); // Remove leading key
        // fields
        this->fields.insert(this->fields.end(), fields.cbegin(), fields.cend());
        SizeType i = 0;
        for (const auto &field : fields) {
            if (field == "KEY")
                throw MultipleKey(
                        "Error creating table \"" + name() + "\": Multiple KEY field."
                );
            fieldMap.emplace(field, i++);
        }

        Table::SizeType lineCount = 2;
        while (std::getline(infile, line)) {
            if (line.empty()) break; // Read to an empty line
            lineCount++;
            sstream.clear();
            sstream.str(line);
            std::string key;
            if (!(sstream >> key)) {
                throw std::runtime_error("Database::fillTableWithFile: Missing or invalid KEY field!");
            }
            std::vector<Table::ValueType> tuple;
            tuple.reserve(fieldCount - 1);
            for (Table::SizeType i = 1; i < fieldCount; ++i) {
                Table::ValueType value;
                if (!(sstream >> value)) {
                    throw std::runtime_error("Database::fillTableWithFile: Invalid row on LINE " + std::to_string(lineCount));
                }
                tuple.emplace_back(value);
            }
            insertByIndex(key, std::move(tuple));
        }
        makeReady();
    }


    /**
     * Copy constructor from another table
     * @param name: the table name (must be unique in the database)
     * @param origin: the original table copied from
     */
    Table(std::string name, const Table &origin) :
            fields(origin.fields), fieldMap(origin.fieldMap), data(origin.data),
            keyMap(origin.keyMap), tableName(std::move(name)) {}

    /**
     * Find the index of a field in the fieldMap
     * @param field
     * @return fieldIndex
     */
    FieldIndex getFieldIndex(const FieldNameType &field) const;

    /**
     * Insert a row of data by its key
     * @tparam ValueTypeContainer
     * @param key
     * @param data
     */
    void insertByIndex(KeyType key, std::vector<ValueType> &&data);

    void insertByAppend(std::vector<Datum> &&v);

    bool existKey(KeyType key) {
      return (this->keyMap.find(key) != this->keyMap.end());
    }

    /**
     * Access the value according to the key
     * @param key
     * @return the Object that KEY = key, or nullptr if key doesn't exist
     */
    Object::Ptr operator[](const KeyType &key);

    /**
     * [For DeleteQuery]
     * Erase the record refered by keys in the vector
     * @param iterator
     * @complexity O(N)
     */
    void erase(const std::vector<KeyType> &keys);

    /**
     * Set the name of the table
     * @param name
     */
    void setName(std::string name) { this->tableName = std::move(name); }

    /**
     * Get the name of the table
     * @return
     */
    const std::string &name() const { return this->tableName; }

    /**
     * Return whether the table is empty
     * @return
     */
    bool empty() const { return this->data.empty(); }

    /**
     * Return the num of data stored in the table
     * @return
     */
    size_t size() const { return this->data.size(); }

    /**
     * Return the fields in the table
     * @return
     */
    const std::vector<FieldNameType> &field() const { return this->fields; }

    /**
     * Clear all content in the table
     * @return rows affected
     */
    size_t clear() {
        auto result = keyMap.size();
        data.clear();
        keyMap.clear();
        return result;
    }

    /**
     * Get a begin iterator similar to the standard iterator
     * @return begin iterator
     */
    Iterator begin() { return {data.begin(), this}; }

    /**
     * Get a end iterator similar to the standard iterator
     * @return end iterator
     */
    Iterator end() { return {data.end(), this}; }

    /**
     * Get a const begin iterator similar to the standard iterator
     * @return const begin iterator
     */
    ConstIterator begin() const { return {data.cbegin(), this}; }

    /**
     * Get a const end iterator similar to the standard iterator
     * @return const end iterator
     */
    ConstIterator end() const { return {data.cend(), this}; }

    /**
     * Overload the << operator for complete print of the table
     * @param os
     * @param table
     * @return the origin ostream
     */
    friend std::ostream &operator<<(std::ostream &os, const Table &table);
};

std::ostream &operator<<(std::ostream &os, const Table &table);

template<class FieldIDContainer>
Table::Table(const std::string &name, const FieldIDContainer &fields)
        : fields(fields.cbegin(), fields.cend()), tableName(name) {
    SizeType i = 0;
    for (const auto &field : fields) {
        if (field == "KEY")
            throw MultipleKey(
                    "Error creating table \"" + name + "\": Multiple KEY field."
            );
        fieldMap.emplace(field, i++);
    }
}

#endif //PROJECT_DB_TABLE_H
