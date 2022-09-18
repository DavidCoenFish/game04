#pragma once

class IUpdate
{
public:
   virtual ~IUpdate();
   virtual void Update(const float in_timeDeltaSeconds) = 0;

};
