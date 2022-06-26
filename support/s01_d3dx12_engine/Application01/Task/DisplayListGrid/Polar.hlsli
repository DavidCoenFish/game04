float3 PolarToVector(float2 polarCoordRadian) //radians
{
   float baseX = 0.0f; //sin(polarCoordRadian.x);
   float baseZ = 0.0f; //cos(polarCoordRadian.x);
   sincos(polarCoordRadian.x, baseX, baseZ);
   float riseBase = 0.0f; //cos(polarCoordRadian.y);
   float y = 0.0f; //sin(polarCoordRadian.y);
   sincos(polarCoordRadian.y, y, riseBase);
   float x = baseX * riseBase;
   float z = baseZ * riseBase;
   return float3(x, y, z);
}

