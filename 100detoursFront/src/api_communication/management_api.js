/*
 *  Utils functions for Management section 
 *
 */

export async function fetch_add_nurse(credentials, handleLogin, set_error)
{
    const response = await fetch('/auth/login', {
                          method: 'POST',
                          headers: { 'Content-Type': 'application/json' },
                          body: JSON.stringify(credentials),
                            })

    try
    {
      let res = await response.json();
      let status = response.status;
      if (status === 200)
      {
          handleLogin(res, credentials);
      }
      else
      {
          set_error(true)
         console.error(`Request rejected with status ${status} and message ${JSON.stringify(res)}`)
      }
    }
    catch (err)
    {
        set_error(true)
        console.error(`Request rejected with status ${response.status}  and message ${JSON.stringify(response)}`)
    }
}




export async function get_nurses_list(set_state_callback, set_error)
{
    const response = await fetch('/nurses')

    if (response.status === 200)
    {
        const nurses_list = response.json()
        set_state_callback( {data: nurses_list});
    }
    else
    {
        set_error("Error trying to get nurses list")
        response.text().then( (text) => 
                     console.error(`Get nurses list request rejected with status ${response.status} and message ${text}`)) 
    }

}

export async function add_nurse(nurse, success_callback, set_error)
{
        const response = await fetch('/manage/add_nurse', {
                                                    method: 'POST',
                                                    headers: { 'Content-Type': 'application/json' },
                                                    body: JSON.stringify(nurse),
                                                        })

        let res = await response;
        if (res.status === 200)
        {
                success_callback(res.json(), nurse);
        }
        else
        {
            set_error(true)
            res.text().then( (text) => 
                         console.error(`Add nurse request rejected with status ${res.status} and message ${text}`))

        }
}

export async function edit_nurse(nurse, success_callback, set_error)
{
        const response = await fetch('/manage/edit_nurse', {
                                                    method: 'POST',
                                                    headers: { 'Content-Type': 'application/json' },
                                                    body: JSON.stringify(nurse),
                                                        })

        let res = await response;
        if (res.status === 200)
        {
                success_callback(res.json(), nurse);
        }
        else
        {
            set_error(true)
            res.text().then( (text) => 
                         console.error(`Edit nurse request rejected with status ${res.status} and message ${text}`))

        }
}

export async function remove_nurse(nurse, success_callback, set_error)
{
        const response = await fetch('/manage/remove_nurse', {
                                                    method: 'POST',
                                                    headers: { 'Content-Type': 'application/json' },
                                                    body: JSON.stringify(nurse),
                                                        })

        let res = await response;
        if (res.status === 200)
        {
                success_callback(res.json(), nurse);
        }
        else
        {
            set_error(true)
            res.text().then( (text) => 
                         console.error(`Delete nurse request rejected with status ${res.status} and message ${text}`))

        }
}

