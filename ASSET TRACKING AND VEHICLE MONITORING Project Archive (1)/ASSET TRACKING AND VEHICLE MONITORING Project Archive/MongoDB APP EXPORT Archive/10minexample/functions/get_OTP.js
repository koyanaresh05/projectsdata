
exports = function(payload){  

    const mongodb = context.services.get("mongodb-atlas");
  const itemsCollection = mongodb.db("ASSET_INFO").collection("OTP_Data");
    // Querying a mongodb service:
 // var doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find();
// const doc = itemsCollection.findOne({"valid":"true"},{_id:0});
  const doc_num_OTP = itemsCollection.findOne({"Mob":"OTP Generator"},{_id:0});
  const doc = itemsCollection.findOne({"valid":"true"},{_id:0});
 
  const error_doc = "{\"Mob\": \"Not_found\"}"; 
   // const name1 = doc.Source;
   if(doc_num_OTP.OTP !="0")
   return  doc;
   else 
 { // return error_doc;
  doc_num_OTP['Mob'] = "Not_found";
  return doc_num_OTP;
 }
};



