//
//  fast_string.c
//  Playground
//
//  Created by Albert Slepak on 9/11/20.
//  Copyright © 2020 none. All rights reserved.
//

#include "fast_string.h"

fast_string::fast_string(size_t size)
: m_Capacity(size)
{
    char* data_ptr = m_SSOBuffer;
    
    // Allocate memory on the heap only if the capacity is over the default size of the SSO buffer.
    if (size > sizeof(m_SSOBuffer))
    {
        // Allocating heap memory
        m_Data = (char*)malloc(m_Capacity);
        
        // Adjusting data_ptr to point to the dynamically allocated memory block
        data_ptr = m_Data;
    }
    
    // Not forgetting the null terminator
    // It is placed as the first character since no actual content exists
    data_ptr[0] = '\0';
    
    // Setting the default length to 0 since no content exists
    m_Length = 0;
}

fast_string::fast_string(const char* init)
{
    // Getting the length
    m_Length = strlen(init);
    
    char* data_ptr = m_SSOBuffer;
    
    // Use heap buffer only if capacity is greater than default size of the SSO buffer.
    if (m_Length > sizeof(m_SSOBuffer) - 1)
    {
        // Set the capacity to 1 more than the length to fit the null-terminator in
        m_Capacity = m_Length + 1;
        
        // Allocate memory just enough to hold the string
        m_Data = (char*)malloc(m_Capacity);
        
        // Adjusting data_ptr to point to the dynamically allocated memory block
        data_ptr = m_Data;
    }
    
    // Copying the contents of the initializer string into the data buffer
    memcpy(data_ptr, init, m_Length);
    
    // Not forgetting the null terminator
    data_ptr[m_Length] = '\0';
}

fast_string::fast_string(const fast_string& other)
{
    // Copy hash, capacity and length members
    m_Capacity = other.capacity();
    m_Length = other.length();
    m_Hash = other.get_hash();
    
    if (m_Capacity > sizeof(m_SSOBuffer))
    {
        // Allocate a new block of memory for the data buffer if capacity is over default size of the SSO buffer
        m_Data = (char*)malloc(m_Capacity);
        
        // Copy all the contents including the null terminator
        memcpy(m_Data, other.m_Data, m_Capacity);
    }
    else
    {
        memcpy(m_SSOBuffer, other.m_SSOBuffer, sizeof(m_SSOBuffer));
    }
}

fast_string::~fast_string()
{
    // Freeing the data buffer
    free(m_Data);
}

void fast_string::generate_hash()
{
    m_Hash = 0x3B6C;
    char* ptr = m_Data;
    int c;
    
    while ((c = *ptr++))
        m_Hash = ((m_Hash << 5) + m_Hash) + c;
}

void fast_string::reserve(size_t bytes)
{
    // Adjust the capacity member
    m_Capacity += bytes;
    
    // Allocate additional memory
    m_Data = (char*)realloc(m_Data, m_Capacity);
}

void fast_string::swap(fast_string& fs)
{
    // Creating temporary variables to hold current string's data
    char*  this_data = m_Data;
    size_t this_hash = m_Hash;
    size_t this_capacity = m_Capacity;
    size_t this_length = m_Length;
    
    char   this_ssobuffer[_default_sso_size];
    memcpy(this_ssobuffer, m_SSOBuffer, _default_sso_size);
    
    // Setting this string's data
    m_Data = fs.m_Data;
    m_Hash = fs.m_Hash;
    m_Capacity = fs.m_Capacity;
    m_Length = fs.m_Length;
    memcpy(m_SSOBuffer, fs.m_SSOBuffer, _default_sso_size);
    
    // Setting the other string's data
    fs.m_Data = this_data;
    fs.m_Hash = this_hash;
    fs.m_Capacity = this_capacity;
    fs.m_Length = this_length;
    memcpy(fs.m_SSOBuffer, this_ssobuffer, _default_sso_size);
}

void fast_string::push_back(char c)
{
    // If the current capacity can't fit in 1 more
    // character, allocate additional memory with realloc().
    if (m_Length + 1 >= m_Capacity)
    {
        bool should_copy_sso_buffer = (m_Capacity == _default_sso_size);
        
        // Adjust the capacity member
        m_Capacity += 1;
        
        // Expand the current memory buffer
        m_Data = (char*)realloc(m_Data, m_Capacity);
        
        // If SSO buffer was previously used, copy its contents into the new heap buffer
        if (should_copy_sso_buffer)
            memcpy(m_Data, m_SSOBuffer, _default_sso_size);
    }
    
    char* data_ptr = (char*)c_str();
    
    // Replace the current null terminator with the new character
    data_ptr[m_Length] = c;
    
    // Adjust the length member
    m_Length += 1;
    
    // Place a new null terminator
    data_ptr[m_Length] = '\0';
}

void fast_string::pop_back()
{
    char* data_ptr = (char*)c_str();
    
    // Adjust the length member
    m_Length -= 1;
    
    // Place a new null terminator
    data_ptr[m_Length] = '\0';
}

void fast_string::append(const fast_string& fs)
{
    // Handling the case when current data buffer already has enough space
    // to fit in the other string at the end.
    if (m_Length + fs.length() < m_Capacity)
    {
        char* data_ptr = (char*)c_str();
        
        // Copying the new string's content to the end of current data buffer
        // Null terminator is accounted for
        memcpy(data_ptr + m_Length, fs.c_str(), fs.length() + 1);
    }
    else
    {
        bool should_copy_sso_buffer = (m_Capacity == _default_sso_size);
        
        // Current capacity is not large enough to fit the new string in.
        m_Data = (char*)realloc(m_Data, m_Length + fs.length() + 1);
        
        // If SSO buffer was previously used, copy its contents into the new heap buffer
        if (should_copy_sso_buffer)
            memcpy(m_Data, m_SSOBuffer, _default_sso_size);
        
        // Copying the new string's content to the end of current data buffer
        memcpy(m_Data + m_Length, fs.c_str(), fs.length() + 1);
        
        // Adjusting the capacity member
        m_Capacity = m_Length + fs.length() + 1;
    }
    
    // Adjusting length member
    m_Length += fs.length();
}

void fast_string::append(const char* str)
{
    uint64_t len = strlen(str);
    
    // Handling the case when current data buffer already has enough space
    // to fit in the other string at the end.
    if (m_Length + len < m_Capacity)
    {
        char* data_ptr = (char*)c_str();
        
        // Copying the new string's content to the end of current data buffer
        // Null terminator is accounted for
        memcpy(data_ptr + m_Length, str, len + 1);
    }
    else
    {
        bool should_copy_sso_buffer = (m_Capacity == _default_sso_size);
        
        // Current capacity is not large enough to fit the new string in.
        m_Data = (char*)realloc(m_Data, m_Length + len + 1);
        
        // If SSO buffer was previously used, copy its contents into the new heap buffer
        if (should_copy_sso_buffer)
            memcpy(m_Data, m_SSOBuffer, _default_sso_size);
        
        // Copying the new string's content to the end of current data buffer
        memcpy(m_Data + m_Length, str, len + 1);
        
        // Adjusting the capacity member
        m_Capacity = m_Length + len + 1;
    }
    
    // Adjusting length member
    m_Length += len;
}

fast_string& fast_string::substr(size_t index, size_t count)
{
    if (index > m_Length)
        throw std::runtime_error("(fast_string error) index out of range");
    
    char* data_ptr = (char*)c_str();
    
    // If count of characters to copy goes over the string's length, copy
    // only maximum available characters.
    size_t available_count = ((count - index) < m_Length) ? count : (m_Length - index);
    
    // Copy the substringed data to the beginning of the buffer
    memcpy(data_ptr, data_ptr + index, available_count);
    
    // Adjust the length member
    m_Length = available_count;
    
    // Adjust the position of the null-terminator
    data_ptr[m_Length] = '\0';
    
    return *this;
}

size_t fast_string::find(const fast_string& substr) const
{
    char* data_ptr = (char*)c_str();
    
    // Linear Search Algorithm
    for (size_t i = 0; i < m_Length; i++)
    {
        if (data_ptr[i] == substr[0])
        {
            bool found = true;
            for (size_t c = 0; c < substr.length(); c++)
            {
                if (data_ptr[i + c] != substr[c])
                {
                    found = false;
                    break;
                }
            }
            
            if (found)
                return i;
        }
    }
    
    return invalid;
}

size_t fast_string::find(const char* substr) const
{
    char* data_ptr = (char*)c_str();
    uint64_t len = strlen(substr);
    
    // Linear Search Algorithm
    for (size_t i = 0; i < m_Length; i++)
    {
        if (data_ptr[i] == substr[0])
        {
            bool found = true;
            for (size_t c = 0; c < len; c++)
            {
                if (data_ptr[i + c] != substr[c])
                {
                    found = false;
                    break;
                }
            }
            
            if (found)
                return i;
        }
    }
    
    return invalid;
}

bool fast_string::equal(const fast_string& fs) const
{
    char* this_data_ptr = (char*)c_str();
    char* fs_data_ptr = (char*)fs.c_str();
    
    // Check if lengths are different
    if (m_Length != fs.length())
        return false;
    
    // Check if first characters are different
    if (this_data_ptr[0] != fs_data_ptr[0])
        return false;
    
    // If hashes exist for both strings and they are not
    // equal, then the strings are not equal either.
    if (m_Hash && fs.m_Hash && (m_Hash != fs.m_Hash))
        return false;
    
    // If previous checks pass, perform strcmp
    return (strcmp(this_data_ptr, fs_data_ptr) == 0);
}

void fast_string::replace(const fast_string& substr, const fast_string& replacement)
{
    // Get the index of the first substring occurence
    size_t index = find(substr);
    if (index != invalid)
    {
        // Here there are two possible cases: either
        // the string's capacity can fit in the replacement,
        // or the string's buffer must be resized using realloc().
        
        // If the current capacity is enough, resize the buffer to the appropriate size
        if (m_Length - substr.length() + replacement.length() > m_Capacity)
        {
            bool should_copy_sso_buffer = (m_Capacity == _default_sso_size);
            
            // Adjust the capacity member
            m_Capacity += (replacement.length() - substr.length());
            
            // Reallocate the memory buffer
            m_Data = (char*)realloc(m_Data, m_Capacity);
            
            // If SSO buffer was previously used, copy its contents into the new heap buffer
            if (should_copy_sso_buffer)
                memcpy(m_Data, m_SSOBuffer, _default_sso_size);
        }
        
        char* data_ptr = (char*)c_str();
        
        // Copy the existing contents after the substring
        // to positions after the future replacement's length.
        memcpy(
               data_ptr + index + replacement.length(),
               data_ptr + index + substr.length(),
               m_Length - (index + substr.length())
               );
        
        // Copy the replacement at index
        memcpy(data_ptr + index, replacement.c_str(), replacement.length());
        
        // Adjust the length member.
        // Casting to a signed integer is required
        // in case length needs to be shrinked.
        m_Length += ((int64_t)replacement.length() - (int64_t)substr.length());
        
        // Placing a null terminator at the correct position
        data_ptr[m_Length] = '\0';
    }
}

void fast_string::replace(const fast_string& substr, const char* replacement)
{
    uint64_t replacement_len = strlen(replacement);
    
    // Get the index of the first substring occurence
    size_t index = find(substr);
    if (index != invalid)
    {
        // Here there are two possible cases: either
        // the string's capacity can fit in the replacement,
        // or the string's buffer must be resized using realloc().
        
        // If the current capacity is enough, resize the buffer to the appropriate size
        if (m_Length - substr.length() + replacement_len > m_Capacity)
        {
            bool should_copy_sso_buffer = (m_Capacity == _default_sso_size);
            
            // Adjust the capacity member
            m_Capacity += (replacement_len - substr.length());
            
            // Reallocate the memory buffer
            m_Data = (char*)realloc(m_Data, m_Capacity);
            
            // If SSO buffer was previously used, copy its contents into the new heap buffer
            if (should_copy_sso_buffer)
                memcpy(m_Data, m_SSOBuffer, _default_sso_size);
        }
        
        char* data_ptr = (char*)c_str();
        
        // Copy the existing contents after the substring
        // to positions after the future replacement's length.
        memcpy(
               data_ptr + index + replacement_len,
               data_ptr + index + substr.length(),
               m_Length - (index + substr.length())
               );
        
        // Copy the replacement at index
        memcpy(data_ptr + index, replacement, replacement_len);
        
        // Adjust the length member.
        // Casting to a signed integer is required
        // in case length needs to be shrinked.
        m_Length += ((int64_t)replacement_len - (int64_t)substr.length());
        
        // Placing a null terminator at the correct position
        data_ptr[m_Length] = '\0';
    }
}

void fast_string::replace(const char* substr, const fast_string& replacement)
{
    uint64_t substr_len = strlen(substr);
    
    // Get the index of the first substring occurence
    size_t index = find(substr);
    if (index != invalid)
    {
        // Here there are two possible cases: either
        // the string's capacity can fit in the replacement,
        // or the string's buffer must be resized using realloc().
        
        // If the current capacity is enough, resize the buffer to the appropriate size
        if (m_Length - substr_len + replacement.length() > m_Capacity)
        {
            bool should_copy_sso_buffer = (m_Capacity == _default_sso_size);
            
            // Adjust the capacity member
            m_Capacity += (replacement.length() - substr_len);
            
            // Reallocate the memory buffer
            m_Data = (char*)realloc(m_Data, m_Capacity);
            
            // If SSO buffer was previously used, copy its contents into the new heap buffer
            if (should_copy_sso_buffer)
                memcpy(m_Data, m_SSOBuffer, _default_sso_size);
        }
        
        char* data_ptr = (char*)c_str();
        
        // Copy the existing contents after the substring
        // to positions after the future replacement's length.
        memcpy(
               data_ptr + index + replacement.length(),
               data_ptr + index + substr_len,
               m_Length - (index + substr_len)
               );
        
        // Copy the replacement at index
        memcpy(data_ptr + index, replacement.c_str(), replacement.length());
        
        // Adjust the length member.
        // Casting to a signed integer is required
        // in case length needs to be shrinked.
        m_Length += ((int64_t)replacement.length() - (int64_t)substr_len);
        
        // Placing a null terminator at the correct position
        data_ptr[m_Length] = '\0';
    }
}

void fast_string::replace(const char* substr, const char* replacement)
{
    uint64_t substr_len = strlen(substr);
    uint64_t replacement_len = strlen(replacement);
    
    // Get the index of the first substring occurence
    size_t index = find(substr);
    if (index != invalid)
    {
        // Here there are two possible cases: either
        // the string's capacity can fit in the replacement,
        // or the string's buffer must be resized using realloc().
        
        // If the current capacity is enough, resize the buffer to the appropriate size
        if (m_Length - substr_len + replacement_len > m_Capacity)
        {
            bool should_copy_sso_buffer = (m_Capacity == _default_sso_size);
            
            // Adjust the capacity member
            m_Capacity += (replacement_len - substr_len);
            
            // Reallocate the memory buffer
            m_Data = (char*)realloc(m_Data, m_Capacity);
            
            // If SSO buffer was previously used, copy its contents into the new heap buffer
            if (should_copy_sso_buffer)
                memcpy(m_Data, m_SSOBuffer, _default_sso_size);
        }
        
        char* data_ptr = (char*)c_str();
        
        // Copy the existing contents after the substring
        // to positions after the future replacement's length.
        memcpy(
               data_ptr + index + replacement_len,
               data_ptr + index + substr_len,
               m_Length - (index + substr_len)
               );
        
        // Copy the replacement at index
        memcpy(data_ptr + index, replacement, replacement_len);
        
        // Adjust the length member.
        // Casting to a signed integer is required
        // in case length needs to be shrinked.
        m_Length += ((int64_t)replacement_len - (int64_t)substr_len);
        
        // Placing a null terminator at the correct position
        data_ptr[m_Length] = '\0';
    }
}

void fast_string::erase(const fast_string& substr)
{
    replace(substr, "");
}

void fast_string::erase(const char* substr)
{
    replace(substr, "");
}

void fast_string::erase(size_t index, size_t count)
{
    if (index > m_Length)
        throw std::runtime_error("(fast_string error) index out of range");
    
    char* data_ptr = (char*)c_str();
    
    // If count of characters to erase goes over the string's length, use
    // only maximum number of available characters.
    size_t available_count = (count < m_Length - index) ? count : (m_Length - index);
    
    // Copy the existing contents after the substring
    // to the substring's index (shift contents back).
    memcpy(
           data_ptr + index,
           data_ptr + index + available_count,
           m_Length - (index + available_count)
           );
    
    // Adjust the length member.
    m_Length -= available_count;
    
    // Placing a null terminator at the correct position
    data_ptr[m_Length] = '\0';
}

void fast_string::insert(size_t index, const fast_string& fs)
{
    if (index > m_Length)
        throw std::runtime_error("(fast_string error) index out of range");
    
    // If the current capacity is not enough, resize the buffer to the appropriate size
    if (m_Length + fs.length() >= m_Capacity)
    {
        bool should_copy_sso_buffer = (m_Capacity == _default_sso_size);
        
        // Adjust the capacity member
        m_Capacity = m_Length + fs.length() + 1;
        
        // Reallocate the memory buffer
        m_Data = (char*)realloc(m_Data, m_Capacity);
        
        // If SSO buffer was previously used, copy its contents into the new heap buffer
        if (should_copy_sso_buffer)
            memcpy(m_Data, m_SSOBuffer, _default_sso_size);
    }
    
    char* data_ptr = (char*)c_str();
    
    // Copy the existing contents after the substring
    // to the substring's index (shift contents forward making space for the insertion).
    memcpy(
           data_ptr + index + fs.length(),
           data_ptr + index,
           m_Length - index
           );
    
    // Copy the new string into the appropriate memory segment
    memcpy(data_ptr + index, fs.c_str(), fs.length());
    
    // Adjust the length member.
    m_Length += fs.length();
    
    // Placing a null terminator at the correct position
    data_ptr[m_Length] = '\0';
}

void fast_string::insert(size_t index, const char* str)
{
    if (index > m_Length)
        throw std::runtime_error("(fast_string error) index out of range");
    
    uint64_t len = strlen(str);
    
    // If the current capacity is not enough, resize the buffer to the appropriate size
    if (m_Length + len >= m_Capacity)
    {
        bool should_copy_sso_buffer = (m_Capacity == _default_sso_size);
        
        // Adjust the capacity member
        m_Capacity = m_Length + len + 1;
        
        // Reallocate the memory buffer
        m_Data = (char*)realloc(m_Data, m_Capacity);
        
        // If SSO buffer was previously used, copy its contents into the new heap buffer
        if (should_copy_sso_buffer)
            memcpy(m_Data, m_SSOBuffer, _default_sso_size);
    }
    
    char* data_ptr = (char*)c_str();
    
    // Copy the existing contents after the substring
    // to the substring's index (shift contents forward making space for the insertion).
    memcpy(
           data_ptr + index + len,
           data_ptr + index,
           m_Length - index
           );
    
    // Copy the new string into the appropriate memory segment
    memcpy(data_ptr + index, str, len);
    
    // Adjust the length member.
    m_Length += len;
    
    // Placing a null terminator at the correct position
    data_ptr[m_Length] = '\0';
}

std::ostream& operator<<(std::ostream& os, const fast_string& fs)
{
    os << fs.c_str();
    return os;
}

fast_string& fast_string::operator=(const fast_string& fs)
{
    // If it's not self-assignment
    if (this != &fs)
    {
        // Should expand data buffer only if current capacity isn't enough
        if (fs.capacity() > m_Capacity)
        {
            // Calling realloc since it is not guaranteed to allocate a brand new pointer
            m_Data = (char*)realloc(m_Data, fs.capacity());
            
            // Updating the capacity member
            m_Capacity = fs.capacity();
        }
        
        char* data_ptr = (char*)c_str();
        
        // Updating the length member
        m_Length = fs.length();
        
        // Copying the data from the new string into the data buffer
        // Since capacity already includes the null terminator byte, it is also copied
        memcpy(data_ptr, fs.c_str(), fs.capacity());
    }
    
    return *this;
}

fast_string& fast_string::operator=(const char* str)
{
    uint64_t len = strlen(str);
    uint64_t capacity = len + 1;
    
    // Should expand data buffer only if current capacity isn't enough
    if (capacity > m_Capacity)
    {
        // Calling realloc since it is not guaranteed to allocate a brand new pointer
        m_Data = (char*)realloc(m_Data, capacity);
        
        // Updating the capacity member
        m_Capacity = capacity;
    }
    
    char* data_ptr = (char*)c_str();
    
    // Updating the length member
    m_Length = len;
    
    // Copying the data from the new string into the data buffer
    // Since capacity already includes the null terminator byte, it is also copied
    memcpy(data_ptr, str, len);
    
    // Updating the null terminator
    data_ptr[m_Length] = '\0';
    
    return *this;
}

fast_string fast_string::operator+(const fast_string& fs)
{
    fast_string result = *this;
    result.append(fs);
    return result;
}

fast_string fast_string::operator+(const char* str)
{
    fast_string result = *this;
    result.append(str);
    return result;
}

fast_string fast_string::operator+(const char c)
{
    fast_string result = *this;
    result.push_back(c);
    return result;
}

fast_string& fast_string::operator+=(const fast_string& fs)
{
    append(fs);
    return *this;
}

fast_string& fast_string::operator+=(const char* str)
{
    append(str);
    return *this;
}

fast_string& fast_string::operator-=(const fast_string& fs)
{
    erase(fs);
    return *this;
}

fast_string& fast_string::operator-=(const char* str)
{
    erase(str);
    return *this;
}

bool fast_string::operator==(fast_string& rhs) const
{
    return equal(rhs);
}

bool fast_string::operator!=(fast_string& rhs) const
{
    return !equal(rhs);
}

char fast_string::operator[](size_t index) const
{
    if (index > m_Length)
        throw std::runtime_error("(fast_string error) index out of range");
    
    return c_str()[index];
}
