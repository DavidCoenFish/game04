#include "CommonPCH.h"
#include "Common/Application/CommandLine.h"
#include "Common/Log/Log.h"

std::shared_ptr< CommandLine > CommandLine::Factory( const std::string& commandLine )
{
   LOG_MESSAGE("CommandLine:");
   std::vector< std::shared_ptr< IToken > > tokenArray;
   {
      Tokeniser tokeniser(tokenArray);
      for (const auto character : commandLine)
      {
         tokeniser.AddCharacter(character);
      }
   }

   return std::make_shared< CommandLine >(tokenArray);
}

CommandLine::TokenFlag::TokenFlag( const std::string& name)
: m_name(name)
{
   //nop
}
const std::string& CommandLine::TokenFlag::GetName() const
{
   return m_name;
}

CommandLine::TokenValueString::TokenValueString( const std::string& name, const std::string& value)
   : m_name(name)
   , m_value(value)
{
   //nop
}
const std::string& CommandLine::TokenValueString::GetName() const
{
   return m_name;
}

const std::string& CommandLine::TokenValueString::GetValue() const
{
   return m_value;
}

CommandLine::TokenValueInt::TokenValueInt( const std::string& name, const int value )
   : m_name(name)
   , m_value(value)
{
   //nop
}
const std::string& CommandLine::TokenValueInt::GetName() const
{
   return m_name;
}
const int CommandLine::TokenValueInt::GetValue() const
{
   return m_value;
}

CommandLine::Tokeniser::Tokeniser(std::vector< std::shared_ptr< IToken > >& tokenArray)
: m_tokenArray(tokenArray)
, m_bInQuote(false)
, m_bInToken(true)
, m_bPrevQuote(false)
, m_token()
, m_value()
{
   //nop
}

CommandLine::Tokeniser::~Tokeniser()
{
   Flush();
}

void CommandLine::Tokeniser::AddCharacter(unsigned char c)
{
   if (m_bInQuote)
   {
      if (c == '"')
      {
         m_bInQuote = false;
         m_bPrevQuote = true;
      }
      else
      {
         PushCharacter(c);
         m_bPrevQuote = false;
      }
   }
   else
   {
      if (0 != std::isspace(c))
      {
         Flush();
      }
      else if (c == '"')
      {
         if (true == m_bPrevQuote)
         {
            PushCharacter('"');
            m_bPrevQuote = false;
         }
         m_bInQuote = true;
      }
      else if (c == '=')
      {
         if (m_bInToken)
         {
            m_bInToken = false;
         }
         else
         {
            PushCharacter(c);
         }
      }
      else
      {
         PushCharacter(c);
      }
   }
}

void CommandLine::Tokeniser::PushCharacter(unsigned char c)
{
   if (m_bInToken)
   {
      m_token += c;
   }
   else
   {
      m_value += c;
   }
}

static const bool IsNumber(std::string& value, int& out_number)
{
   out_number = 0;
   bool bNegative = false;
   bool bAllDigits = true;
   int index = 0;
   if (value[index] == '-')
   {
      index += 1;
      bNegative = true;
      if (1 == (int)value.size())
      {
         return false;
      }
   }
   while(index < (int)value.size())
   {
      unsigned char character = value[index];
      index += 1;
      switch(character)
      {
      default:
         return false;
      case '0':
         out_number *= 10;
         break;
      case '1':
         out_number = (out_number * 10) + 1;
         break;
      case '2':
         out_number = (out_number * 10) + 2;
         break;
      case '3':
         out_number = (out_number * 10) + 3;
         break;
      case '4':
         out_number = (out_number * 10) + 4;
         break;
      case '5':
         out_number = (out_number * 10) + 5;
         break;
      case '6':
         out_number = (out_number * 10) + 6;
         break;
      case '7':
         out_number = (out_number * 10) + 7;
         break;
      case '8':
         out_number = (out_number * 10) + 8;
         break;
      case '9':
         out_number = (out_number * 10) + 9;
         break;
      }
   }

   if (true == bNegative)
   {
      out_number = -out_number;
   }
   return bAllDigits;
}


void CommandLine::Tokeniser::Flush()
{
   if (false == m_token.empty())
   {
      int number = 0;
      if (true == m_value.empty())
      {
         LOG_MESSAGE(" %s", m_token.c_str());
         m_tokenArray.push_back(std::make_shared< TokenFlag >( m_token ));
      }
      else if (true == IsNumber(m_value, number))
      {
         LOG_MESSAGE(" %s=%d", m_token.c_str(), number);
         m_tokenArray.push_back(std::make_shared< TokenValueInt >( m_token, number ));
      }
      else
      {
         LOG_MESSAGE(" %s=%s", m_token.c_str(), m_value.c_str());
         m_tokenArray.push_back(std::make_shared< TokenValueString >( m_token, m_value ));
      }
   }

   m_token.clear();
   m_value.clear();
   m_bInQuote = false;
   m_bInToken = true;
   m_bPrevQuote = false;
}


CommandLine::CommandLine( const std::vector< std::shared_ptr< IToken > >& tokenArray )
   : m_paramArray(tokenArray)
{
   for(const auto& iter : tokenArray)
   {
      m_tokenMap[iter->GetName()] = iter;
   }
}

CommandLine::~CommandLine()
{
   //nop
}

const int CommandLine::GetParamCount() const
{
   return (int)m_paramArray.size();
}
const std::string CommandLine::GetParam(const int index) const
{
   if ((0 <= index) && (index < (int)m_paramArray.size()))
   {
      return m_paramArray[index]->GetName();
   }
   return std::string();
}
const bool CommandLine::GetFlag(const std::string& flag) const
{
   auto found = m_tokenMap.find(flag);
   return (found != m_tokenMap.end());
}

const bool CommandLine::GetValueString(const std::string& token, std::string& out_value) const
{
   auto found = m_tokenMap.find(token);
   if (found != m_tokenMap.end())
   {
      auto pTokenValue = dynamic_cast< TokenValueString* >( found->second.get() );
      if (nullptr != pTokenValue)
      {
         out_value = pTokenValue->GetValue();
         return true;
      }
   }
   return false;
}

const bool CommandLine::GetValueInt(const std::string& token, int& out_value) const
{
   auto found = m_tokenMap.find(token);
   if (found != m_tokenMap.end())
   {
      auto pTokenValue = dynamic_cast< TokenValueInt* >( found->second.get() );
      if (nullptr != pTokenValue)
      {
         out_value = pTokenValue->GetValue();
         return true;
      }
   }
   return false;
}
