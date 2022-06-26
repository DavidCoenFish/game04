#pragma once

class JobFinishedToken;

/*
trigger callback when ref count reaches zero, 
put JobFinishedToken into "n" callbacks that you are waiting for, when last token is destroyed, callback is called (in that same thread, so lite work only for callback)
*/
class JobFinished
{
private:
   //disabled
   JobFinished(const JobFinished&);

public:
   // don't put heavy work in the callback, it will just run in the same thread as the last thing to call dtor JobFinished
   static std::shared_ptr<JobFinished> Factory(const std::function<void(void)>& callback);
//private: please use factory rather than class directly, std::make_shared blocking(?) from making ctor private
   JobFinished(const std::function<void(void)>& callback);
//public:
   ~JobFinished();

   //having issue where lambda don't want to assign null to a copy captured smart ptr (as it does a const copy), so add a token "wrapper"
   std::shared_ptr< JobFinishedToken > MakeToken();

private:
   void SetSelfWeakRef(const std::weak_ptr< JobFinished >& weakThis);

private:
   const std::function<void(void)> m_callback;
   std::weak_ptr< JobFinished > m_weakThis;

};
