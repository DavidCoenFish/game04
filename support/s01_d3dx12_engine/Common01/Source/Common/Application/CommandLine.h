#pragma once

class CommandLine
{
public:
   static std::shared_ptr< CommandLine > Factory( const std::string& commandLine );

private:
   class IToken
   {
   public:
      virtual const std::string& GetName() const = 0;
   };
   class TokenFlag : public IToken
   {
   public:
      explicit TokenFlag( const std::string& name = std::string());
      virtual const std::string& GetName() const override;
   private:
      std::string m_name;
   };
   class TokenValueString : public IToken
   {
   public:
      explicit TokenValueString( const std::string& name = std::string(), const std::string& value = std::string() );
      virtual const std::string& GetName() const override;
      const std::string& GetValue() const;
   private:
      std::string m_name;
      std::string m_value;
   };
   class TokenValueInt : public IToken
   {
   public:
      explicit TokenValueInt( const std::string& name = std::string(), const int value = 0 );
      virtual const std::string& GetName() const override;
      const int GetValue() const;
   private:
      std::string m_name;
      int m_value;
   };
   class Tokeniser
   {
   public:
      Tokeniser(std::vector< std::shared_ptr< IToken > >& tokenArray);
      ~Tokeniser();
      void AddCharacter(unsigned char c);
   private:
      void PushCharacter(unsigned char c);
      void Flush();
   private:
      std::vector< std::shared_ptr< IToken > >& m_tokenArray;
      bool m_bInQuote;
      bool m_bInToken;
      bool m_bPrevQuote;
      std::string m_token;
      std::string m_value;
   };

public:
   explicit CommandLine( const std::vector< std::shared_ptr< IToken > >& tokenArray = std::vector< std::shared_ptr< IToken > >() );
   ~CommandLine();

   const int GetParamCount() const;
   const std::string GetParam(const int index) const;
   const bool GetFlag(const std::string& flag) const;
   const bool GetValueString(const std::string& token, std::string& out_value) const;
   const bool GetValueInt(const std::string& token, int& out_value) const;

private:
   std::vector< std::shared_ptr< IToken > > m_paramArray;
   std::map< std::string, std::shared_ptr< IToken > > m_tokenMap;

};
