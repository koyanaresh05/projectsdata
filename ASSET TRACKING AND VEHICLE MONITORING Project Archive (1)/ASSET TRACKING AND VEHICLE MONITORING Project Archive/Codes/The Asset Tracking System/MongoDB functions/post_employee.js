exports = function(payload){
    var atlas = context.services.get("mongodb-atlas");
    var coll = atlas.db("ASSET_INFO").collection("Employee data");
    
      if (payload.body) 
      {
        //const ts = {ts:new Date()};
        //const ts_ej = EJSON.stringify(ts);
        body = EJSON.parse(payload.body.text());
        body['ts'] = new Date();
      }
      coll.insertOne(body);
      console.log(body);
      
 const doc = coll.findOne({"Employee_id":"Get_Employee_Index"},{_id:0});
   const index = parseInt(doc.Employee_index) + 1;
   const query = { "Employee_id": "Get_Employee_Index"};
   const update = {
    "$set": {
      "Employee_index": index
    }
    
  };

    const options = { "upsert": false };// upsert  means update or (insert if field unavaible)

   coll.updateOne(query, update, options);
  

  /*For Inserting document for new location SMS */
    var atlas1 = context.services.get("mongodb-atlas");
    
    var coll1 = atlas1.db("ASSET_INFO").collection("OTP_Data");
   
    var doc2 = coll1.findOne({"Mob":"OTP Generator"},{_id:0});
    var num_OTP = parseInt(doc2.OTP) + 1 ;
    num_OTP = num_OTP + "";// for converting into string
    
    const query2 = { "Mob":"OTP Generator"};
    const update2 = {
      "$set": {
        "OTP": num_OTP
      }
    };
    const options2 = { "upsert": false };// upsert  means update or (insert if field unavaible)
    coll1.updateOne(query2, update2, options2);//update the incremented num_OTP
   const doc3 = EJSON.parse(payload.body.text());
    var SMS_str ="You are successfully registered as an employee. Your EMP ID is : " + doc3.Employee_id;
    
   doc2["Mob"]=doc3.Contact;
   doc2["OTP"]=SMS_str;
      doc2['valid'] = "true"; // This is a dummy key which will help to find latest earliest doc with mob and otp in get_OTP fun
       doc2['ts'] = new Date();
        

      coll1.insertOne(doc2);//Insert new doc containing the new OTP and mob no 
      
  

};
