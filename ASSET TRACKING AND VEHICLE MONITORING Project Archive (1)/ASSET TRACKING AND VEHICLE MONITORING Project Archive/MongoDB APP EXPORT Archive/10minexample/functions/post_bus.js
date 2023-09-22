exports = function(payload){
  
  /*For Inserting document for Bus*/
    var atlas = context.services.get("mongodb-atlas");
    var coll = atlas.db("BUS_INFO").collection("Bus_Data");
    const itemsCollection = atlas.db("ASSET_INFO").collection("New_Luggage_data");
  const doc2 = itemsCollection.findOne({"module":"GPS"}); 
  const last_loc_str = doc2.last_loc;
  
    var db = atlas.db("BUS_INFO");
    const doc = EJSON.parse(payload.body.text());
    const Dist_Travelled_val = doc.Dist_Travelled;
  //  doc['new_dist'] = parseInt(Dist_Travelled_val);
    doc['Dist_Travelled'] =   parseInt(Dist_Travelled_val);
    doc['Engine_Speed'] =     parseInt(doc.Engine_Speed);
    doc['Vehicle_Speed'] =    parseInt(doc.Vehicle_Speed);
    doc['Coolant_Temp'] =     parseInt(doc.Coolant_Temp);
    doc['Engine_Run_Time'] =  parseInt(doc.Engine_Run_Time);
    doc['Throttle_Position'] =parseInt(doc.Throttle_Position); 
    doc['IAT'] =              parseInt(doc.IAT);
    doc['MAF'] =              parseInt(doc.MAF);
    doc['MAP'] =              parseInt(doc.MAP);
    doc['Fuel_Tank_Level']=   parseInt(doc.Fuel_Tank_Level);
    doc["location"]       =   last_loc_str;

 /*   doc.Dist_Travelled = {
   $toInt: Dist_Travelled_val
};*/
//{ $convert:{ input: Dist_Travelled_val, to: "int"}};
    doc['ts'] = new Date();
    coll.insertOne(doc);
    
   
  /*  
    try {
      if (payload.body) 
      {
        //const ts = {ts:new Date()};
        //const ts_ej = EJSON.stringify(ts);
        body = EJSON.parse(payload.body.text());
        const doc = EJSON.parse(payload.body.text());
        const Dist_Travelled_val = doc.Dist_Travelled;

        body['ts'] = new Date();
        body['Dist_Travelled'] = {
   $convert:
      {
         input: Dist_Travelled_val,
         to: $toDecimal

      }
};
      }
      coll.insertOne(body);
      console.log(body);

    } catch (e) {
      console.log("Error inserting doc: " + e);
      return e.message();
    }
    */
};