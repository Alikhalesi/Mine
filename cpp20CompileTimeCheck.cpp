// testcpp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>


enum class ErrorCode
{
    CS001,
    CS002,
    CS003,
    CS004
};

class ErrorToMessage
{
public:
    constexpr ErrorToMessage(ErrorCode errcode, const char* errMsg)
        :msg_{errMsg},code_{errcode}
    {
    }
    constexpr ErrorToMessage(const ErrorToMessage& other) = default;
     

    [[nodiscard]] constexpr const char* GetMsg() const 
    {
        return msg_;
    }
    [[nodiscard]] constexpr ErrorCode GetErrorCode() const
    {
        return code_;
    }

private:
    const char* msg_;
    ErrorCode code_;
};

constexpr ErrorToMessage ErrorLookup[] = {
   {ErrorCode::CS001,"bad cast"},
   {ErrorCode::CS002,"the function not found #S in class #I"},
    {ErrorCode::CS003,"the function not found #S"},
};




template <int index, typename Arg,typename ...Args>
struct TypeArray;

template <int index, typename Arg,typename ...Args>
struct TypeArray
{
    using type =typename TypeArray<index - 1, Args...>::type;
};

template <typename Arg,typename ...Args>
struct TypeArray<0, Arg,Args...>
{
    using type =Arg;
};



template <int ArgCount,bool IsLast,typename ...Args>
struct check
{
    constexpr bool static Check(std::string_view str, std::string::size_type& indexToEndFind)
    {

        indexToEndFind = str.find_last_of('#', indexToEndFind);
        if (indexToEndFind == std::string_view::npos)
        {
            throw;
        }
        switch (const auto c = str.at(++indexToEndFind))
        {
        case 'I':
        {
            const bool res = std::constructible_from<int, typename TypeArray<ArgCount-1, Args...>::type>;
            if (!res)
            {
                throw;
            }
            break;
        }
        case 'S':
        {
            const bool res = std::constructible_from<std::string, typename TypeArray<ArgCount-1, Args...>::type>;
            if (!res)
            {
                throw;
            }
            break;
        }
        default:
        {
            throw;
        }
        }
        indexToEndFind -=2;
        check<ArgCount - 1,true, Args...>::Check(str, indexToEndFind);
        return true;
    }
};


template <typename ...Args>
struct check<0,true,  Args...>
{
    constexpr bool static Check(std::string_view str, std::string::size_type& indexToEndFind)
    {
         return true;
    }
};
template <typename ...Args>
struct check<0, false, Args...>
{
    constexpr bool static Check(std::string_view str,std::string::size_type& indexToEndFind)
    {
        const auto hasParam = str.find_first_of('#');
        if (hasParam != std::string_view::npos)
        {
            throw;
        }
        return true;
    }
};

template <typename ...Args>
struct Checker {
    
    consteval  Checker(ErrorCode code)
	    :code_{code}
	{

        const auto errorString = ErrorLookup[static_cast<int>(code)];
        const auto str = std::string_view{ errorString.GetMsg() };
        std::string::size_type indexToEndFind = str.length();

    	if(!check<sizeof...(Args),false, Args...>::Check( str, indexToEndFind))
        {
            throw;
        }
             
        
    }
    ErrorCode GetErrorCode() const
    {
        return code_;
    }

private:
    ErrorCode code_;
};
template <typename ...Args>
void showError(std::type_identity_t<Checker<Args...>> errCode,Args... args)
{
    const auto errorString = ErrorLookup[static_cast<int>(errCode.GetErrorCode())];
    const auto str = std::string_view{ errorString.GetMsg() };
    std::cout << str << " :\n";
    ((std::cout << args << ','), ...);
    std::cout <<"\n";
};



int main()
{
    showError(ErrorCode::CS001);
    showError(ErrorCode::CS002, "1", 2);
    showError(ErrorCode::CS003,"3");
    system("pause");

}

