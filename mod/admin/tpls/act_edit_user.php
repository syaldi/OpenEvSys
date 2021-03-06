<?php
    global $conf;
    $username = $user->getUserName();
?>

<h2><?php echo _t('EDIT_USER') . " : <span class='red'> $username </span>" ?></h2>
<br />

<?php include "act_edit_user_tabs.php" ?>

<div class='panel'>
    <?php $fields = shn_form_get_html_fields($user_form); ?>
    <div class="form-container"> 
        <form class="form-horizontal"  action='<?php echo get_url('admin', 'edit_user') ?>' method='post'>
           <div class="control-group">
                <div class="controls">
      <a class="btn" href="<?php get_url('admin', 'user_management') ?> " ><i class="icon-remove-circle"></i> <?php echo _t('CANCEL') ?></a>
              
                    <button type="submit" class="btn btn-primary" name="save" ><i class="icon-ok icon-white"></i> <?php echo _t('SAVE') ?></button>

                </div></div> <?php echo $fields['username'] ?>
            <?php echo $fields['first_name'] ?>
            <?php echo $fields['last_name'] ?>
            <?php echo $fields['organization'] ?>
            <?php echo $fields['designation'] ?>
            <?php echo $fields['email'] ?>
            <?php echo $fields['address'] ?>
            <?php echo $fields['locale'] ?>
            <?php echo $fields['role'] ?>
            <?php echo $fields['status'] ?>      

            <div class="control-group">
                <div class="controls">
      <a class="btn" href="<?php get_url('admin', 'user_management') ?> " ><i class="icon-remove-circle"></i> <?php echo _t('CANCEL') ?></a>
              
                    <button type="submit" class="btn btn-primary" name="save" ><i class="icon-ok icon-white"></i> <?php echo _t('SAVE') ?></button>

                </div></div> </form>
    </div>
</div>

