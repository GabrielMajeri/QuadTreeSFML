#pragma once

/**
* This class uses C++11's = delete to delete generated copy
* constructor and copy operator.
*/
class NonCopyable
{
protected:
    NonCopyable() = default;

private:
    NonCopyable(NonCopyable&&) = delete;
    NonCopyable(NonCopyable const&) = delete;

    NonCopyable& operator=(NonCopyable&&) = delete;
    NonCopyable& operator=(NonCopyable const&) = delete;
};
