#pragma once

class IUISystemDataSource;

/*
so if a mod updates the file system overlays, how does the ui system know to reload (data?) files
the file system count have some whay or registering overlay change for file path?
the UISystem could be told that the file system/mod changed. this introduces coupling, why should the mod or ui system know about each other

so, say screen had named animation they could play
and when "FadeBlackToTransparent" finished on the fade screen, you want input to be allowed and the selection 

*/
class IUISystem
{
public:
   virtual ~IUISystem();

   virtual void AddRenderLayer(
      const std::string& renderLayer
      ) = 0;

   virtual void AddScreen(
      const std::string& screenID,
      const std::string& templateName,
      const std::string& renderLayer,
      const std::shared_ptr< IUISystemDataSource >& pDataSource
      ) = 0;

   virtual void RemoveScreen(
      const std::string& screenID
      ) = 0;

   //callback is called at the end of animation
   // if no animation, callback is called instantly
   // if a new animation is triggered before current finished, the currrent callback is call before starting the new animation
   virtual void PlayScreenAnimation(
      const std::string& screenID, 
      const std::string& animationName,
      const std::function<void>& callbackFinished 
      ) = 0;

   virtual void SetNoScreenWithInput() = 0;
   virtual void SetScreenWithInput(const std::string& screenID) = 0;

};
