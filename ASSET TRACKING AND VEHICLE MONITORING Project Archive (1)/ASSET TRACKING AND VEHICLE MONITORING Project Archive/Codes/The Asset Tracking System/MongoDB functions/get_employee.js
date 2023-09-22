
exports = function(payload){  

    const mongodb = context.services.get("mongodb-atlas");
  const itemsCollection = mongodb.db("ASSET_INFO").collection("Employee data");
    // Querying a mongodb service:
   //  const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").findOne();
  //   const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find();
  
  //   const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find({"Asset Status":"Safe"});
  
   //  const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find({"Asset Status":"UnSafe"});  
  // var doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find({"Tag_id":payload.body.text()});
   // doc = doc._id.toString();

 // var doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find({"Tag_id":payload});
 const doc = itemsCollection.findOne({"Employee_id":payload.body.text()},{_id:0});
 // const doc = itemsCollection.find({"type":"unsafe luggage data"},{_id:0});
 if(payload.body.text()=="Get_Employee_Index")
 {
  return doc.Employee_index;
 }
 else
   // const name1 = doc.Source;
   return  doc;
};
