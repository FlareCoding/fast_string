//
//  FastString.h
//  Playground
//
//  Created by Albert Slepak on 9/11/20.
//  Copyright © 2020 none. All rights reserved.
//

#ifndef FastString_h
#define FastString_h
#include <cinttypes>
#include <iostream>
#include <memory>
#include <exception>

class fast_string
{
    constexpr static size_t _default_sso_size = 32;
    
    // Short-String-Optimization buffer for short strings,
    // so they don't get allocated on the heap.
    char m_SSOBuffer[_default_sso_size];
    
    // Data buffer
    char* m_Data = 0;
    
    // Size of the memory allocated in bytes
    // Capacity includes the null-terminator byte
    uint64_t m_Capacity = _default_sso_size;
    
    // Length of the string's content (0 by default)
    uint64_t m_Length = 0;
    
    // Optional hash that the user can generate
    uint64_t m_Hash = 0;
    
public:
    fast_string(size_t capacity = 32);
    fast_string(const char* init);
    fast_string(const fast_string& other);
    ~fast_string();
    
    /// Represents an invalid position index.
    static const size_t invalid = -1;
    
    /// Generates a unique numeric hash for the string.
    void generate_hash();
    
    /// Returns a unique numeric hash specific to this string.
    inline const uint64_t get_hash() const { return m_Hash; }
    
    /// Returns the null-terminated char* string.
    inline const char* c_str() const { return (m_Capacity > _default_sso_size) ? m_Data : m_SSOBuffer; }
    
    /// Returns the string's allocated buffer size that includes the null-terminator.
    inline const uint64_t capacity() const { return m_Capacity; }
    
    /// Returns the length of the actual string's content (does NOT include the null terminator).
    inline const uint64_t length() const { return m_Length; }
    
    /// Returns true if the string's length is 0.
    inline const bool empty() const { return !m_Length; }
    
    /// Increases the capacity, reserving a given number of bytes.
    void reserve(size_t bytes);
    
    /// Swaps the hashes, capacities and string contents of two strings.
    void swap(fast_string& fs);
    
    /// Appends a character to the end of the string.
    void push_back(char c);
    
    /// Erases the last character if the length of string is not 0.
    void pop_back();
    
    /// Adds the contents of the parameter string to the end of the current content.
    /// *Note: No new allocation occurs if current capacity is able to fit in the new content.
    void append(const fast_string& fs);
    
    /// Adds the contents of the parameter string to the end of the current content.
    /// *Note: No new allocation occurs if current capacity is able to fit in the new content.
    void append(const char* str);
    
    /// Replaces own content with its own substring without changing the capacity.
    /// @param index Tells from which character to start reading the substring.
    /// @param count Tells how many characters the substring is. If the count is greater than
    /// the string's length, the maximum available characters are used as substring.
    fast_string& substr(size_t index, size_t count);
    
    /// Returns the index of the first character of first occurence of the substring.
    /// *Note: will return fast_string::invalid if the substring was not found.
    size_t find(const fast_string& substr) const;
    
    /// Returns the index of the first character of first occurence of the substring.
    /// *Note: will return fast_string::invalid if the substring was not found.
    size_t find(const char* substr) const;
    
    /// Returns true if the two strings are equal.
    bool equal(const fast_string& fs) const;
    
    //
    // **Note**
    // The reason there is a replace() function for each variation of arguments
    // is to avoid allocating memory when implicitly converting from
    // char* to fast_string and invoking a constructor.
    //
    
    /// Replaces the first occurence of the substring with a given string.
    void replace(const fast_string& substr, const fast_string& replacement);
    
    /// Replaces the first occurence of the substring with a given string.
    void replace(const fast_string& substr, const char* replacement);
    
    /// Replaces the first occurence of the substring with a given string.
    void replace(const char* substr, const fast_string& replacement);
    
    /// Replaces the first occurence of the substring with a given string.
    void replace(const char* substr, const char* replacement);
    
    /// Erases the first occurence of a specified substring if it exists.
    void erase(const fast_string& substr);
    
    /// Erases the first occurence of a specified substring if it exists.
    void erase(const char* substr);
    
    /// Erases the provided number of characters at a given index.
    /// @param index Tells from which character to start erasing.
    /// @param count Tells how many characters to erase. If the count is greater than
    /// the string's length, the maximum available characters are erased.
    void erase(size_t index, size_t count);
    
    /// Inserts the given string at a specifies index.
    /// @param index Specifies the index at which to insert the new string.
    /// @param fs Specifies the string to insert.
    void insert(size_t index, const fast_string& fs);
    
    /// Inserts the given string at a specifies index.
    /// @param index Specifies the index at which to insert the new string.
    /// @param str Specifies the string to insert.
    void insert(size_t index, const char* str);
    
    friend std::ostream& operator<<(std::ostream& os, const fast_string& fs);
    fast_string& operator=(const fast_string& fs);
    fast_string& operator=(const char* str);
    fast_string operator+(const fast_string& fs);
    fast_string operator+(const char* str);
    fast_string operator+(const char c);
    fast_string& operator+=(const fast_string& fs);
    fast_string& operator+=(const char* str);
    fast_string& operator-=(const fast_string& fs);
    fast_string& operator-=(const char* str);
    bool operator==(fast_string& rhs) const;
    bool operator!=(fast_string& rhs) const;
    char operator[](size_t index) const;
};

#endif /* FastString_h */
