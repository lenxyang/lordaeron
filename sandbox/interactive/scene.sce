<scene name="sandbox_scene_config">
  <node name="root">
   <node name="point" type="light">
     <location>
       <position>0.0f, 10.0f, 0.0f</position>
     </location>
     
     <light type="point_light">
       <ambient>0.1, 0.1, 0.1, 1.0</ambient>
       <diffuse>0.4, 0.4, 0.4, 1.0</diffuse>
       <specular>0.8, 0.8, 0.8, 1.0</specular>
       <range>100.0</range>
     </light>
   </node>
   <node name="teaport" type="mesh">
     <location>
       <position>0.0f, 0.0f, -3.0f</position>
     </location>
     <mesh>
       <path>//model/teapot.obj</path>
       <effect name="DiffuseEffect"/>
       <provider name="lord::DiffuseEffectProvider">
          <color>0.3f, 0.3f, 0.3f, 1.0f</color>
       </provider>
     </mesh>
   </node>
   <node name="tree" type="mesh">
     <location>
       <position>-10.0f, 0.0f, -7.0f</position>
     </location>
     <mesh>
       <path>//model/trunk.obj</path>
       <effect name="DiffuseEffect"/>
       <provider name="lord::DiffuseEffectProvider">
          <color>0.3f, 0.3f, 0.3f, 1.0f</color>
       </provider>
     </mesh>
   </node>
   <node name="venusm" type="mesh">
     <location>
       <!--<scale>0.002, 0.002, 0.002</scale>-->
       <position>10.0f, 0.0f, 0.0f</position>
     </location>
     <mesh>
       <path>//model/cow.obj</path>
       <effect name="DiffuseEffect"/>
       <provider name="lord::DiffuseEffectProvider">
         <color>0.3f, 0.3f, 0.3f, 1.0f</color>
       </provider>
     </mesh>
   </node>
  </node>
</scene>