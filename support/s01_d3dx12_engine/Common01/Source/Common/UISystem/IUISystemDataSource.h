#pragma once

/*
the IUiSystemDataSource is owned by the screen, so no problems with scope?
at most instances where key was used, have added index. this is weak attempt at supporting tables?
how to get the count of the table rows, or use arrays?


AddGeneratorText
AddGeneratorTextArray

*/
class IUISystemTextVisitor;
/*
NewLine()
TextRun() text, colorOveride, fontSizeOverride, styleOveride
Image() imageName, widthPixels, heightPixels, pixelsImageBottomAboveBaseLine
Whitespace() widthPixels
Clear()

*/
class IUISystemTableVisitor;
/*
init(totalRows, arrayColumnMetaData(type, names), callbackRowVisitor?)
*/
class IUISystemRowVisitor;
/*
int GetRangeLow()
int GetRangeHigh()
sumbitRow<Int/String/TextVisitor>

how to select which row to get data of, methods return index
*/

class IUISystemDataSource
{
public:
   virtual ~IUISystemDataSource();

   virtual void OnButtonClick(
      const std::string& key,
      const int index,
      const std::string& verb,
      const std::string& object
      ) = 0;

   //virtual void OnInputString(
   //   const std::string& key,
   //   const std::string& input
   //   ) = 0;

   //virtual void OnMoveSlider(
   //   const std::string& key,
   //   const float x,
   //   const float y
   //   ) = 0;

   virtual const bool GetBool(
      const std::string& key,
      const int index
      ) = 0;
   virtual const bool GetInt(
      const std::string& key,
      const int index
      ) = 0;

   //virtual const std::string GetString(const std::string& key) = 0;
   virtual void SubmitTextVisitor(
      const std::string& key,
      const int index,
      IUISystemTextVisitor& visitor
      ) = 0;

   struct ButtonData
   {
      std::string m_verb;
      std::string m_object;
#if 1
      std::string m_contentKey; 
      int m_contentFlag; 
      //has enable bool
      //has enabled bool generator
      //has string
      //has text
      //has text generator
#else
      std::string m_enabledGeneratorKey;
      std::string m_stringGeneratorKey;
      std::string m_textGeneratorKey;
#endif
   };
   virtual const ButtonData GetButtonData(
      const std::string& key,
      const int index
      ) = 0;
   virtual void GetButtonDataArray(
      const std::string& key,
      std::vector< ButtonData >& buttonDataArray
      ) = 0;

   
   virtual void AddGeneratorBool(
      const std::string& key, 
      const int index,
      const std::function<const bool()>& callback
      ) = 0;
   //RemoveGenerator?
   virtual void AddGeneratorString(
      const std::string& key, 
      const int index,
      const std::function<const std::string()>& callback
      ) = 0;
   virtual void AddGeneratorText(
      const std::string& key, 
      const int index,
      const std::shared_ptr< IUISystemTextVisitor >& visitor
      ) = 0;



};
