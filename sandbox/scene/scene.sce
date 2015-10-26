<scene name="sandbox_scene_config">
  <node name="root">
   <node name="teaport" type="mesh">
     <location>
       <position>0.0f, 0.0f, 0.0f</position>
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
       <position>0.0f, 0.0f, 0.0f</position>
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
       <scale>0.002, 0.002, 0.002</scale>
       <position>10.0f, 0.0f, 0.0f</position>
     </location>
     <mesh>
       <path>//model/venusm.obj</path>
       <effect name="DiffuseEffect"/>
       <provider name="lord::DiffuseEffectProvider">
         <color>0.3f, 0.3f, 0.3f, 1.0f</color>
       </provider>
     </mesh>
   </node>
  </node>
</scene>