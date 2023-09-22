
exports = function(payload){  

    const mongodb = context.services.get("mongodb-atlas");
  const itemsCollection = mongodb.db("ASSET_INFO").collection("New_Luggage_data");
    // Querying a mongodb service:
   //  const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").findOne();
  //   const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find();
  
  //   const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find({"Asset Status":"Safe"});
  
   //  const doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find({"Asset Status":"UnSafe"});  
  // var doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find({"Tag_id":payload.body.text()});
   // doc = doc._id.toString();

 // var doc = context.services.get("mongodb-atlas").db("ASSET_INFO").collection("New_Luggage_data").find({"Tag_id":payload});
 const doc = itemsCollection.find({"Asset Status":"Unsafe"},{_id:0,index:0,Source:0,Destination:0,Contact:0,"Asset Status":0,Password:0,ts:0});
   // const name1 = doc.Source;

//var x= doc.index;
// var y = parseInt(x) + 3;
   return  doc;
};
